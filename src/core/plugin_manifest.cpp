#include "core/plugin_manifest.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <string>
#include <utility>

namespace airmon {
namespace {

struct SemVer {
  int major{};
  int minor{};
  int patch{};

  friend bool operator==(const SemVer&, const SemVer&) = default;
  friend bool operator<(const SemVer& a, const SemVer& b) noexcept {
    if (a.major != b.major) return a.major < b.major;
    if (a.minor != b.minor) return a.minor < b.minor;
    return a.patch < b.patch;
  }
};

bool parseSemVer(std::string_view text, SemVer* value) {
  if (!value || text.empty()) return false;
  SemVer parsed;
  std::array<int*, 3> fields{&parsed.major, &parsed.minor, &parsed.patch};
  std::size_t start = 0;
  for (std::size_t i = 0; i < fields.size(); ++i) {
    const auto end = text.find('.', start);
    const auto part = text.substr(
        start, end == std::string_view::npos ? text.size() - start : end - start);
    if (part.empty() ||
        !std::all_of(part.begin(), part.end(), [](unsigned char ch) {
          return std::isdigit(ch) != 0;
        })) {
      return false;
    }
    try {
      *fields[i] = std::stoi(std::string(part));
    } catch (...) {
      return false;
    }
    if (i < fields.size() - 1) {
      if (end == std::string_view::npos) return false;
      start = end + 1;
    } else if (end != std::string_view::npos) {
      return false;
    }
  }
  *value = parsed;
  return true;
}

bool isSafePluginId(std::string_view id) {
  if (id.empty() || id.front() == '.' || id.back() == '.') return false;
  return std::all_of(id.begin(), id.end(), [](unsigned char ch) {
    return std::isalnum(ch) != 0 || ch == '.' || ch == '-' || ch == '_';
  });
}

bool isRelativeSafePath(std::string_view path) {
  if (path.empty() || path.front() == '/' || path.front() == '\\') return false;
  if (path.size() >= 2 &&
      std::isalpha(static_cast<unsigned char>(path[0])) != 0 &&
      path[1] == ':') {
    return false;
  }
  std::size_t start = 0;
  while (start <= path.size()) {
    const auto end = path.find_first_of("/\\", start);
    const auto part = path.substr(
        start, end == std::string_view::npos ? path.size() - start : end - start);
    if (part == "..") return false;
    if (end == std::string_view::npos) break;
    start = end + 1;
  }
  return true;
}

bool hasCapability(const PluginManifest& manifest, PluginCapability capability) {
  return std::find(manifest.capabilities.begin(), manifest.capabilities.end(),
                   capability) != manifest.capabilities.end();
}

}  // namespace

std::string_view pluginCapabilityName(PluginCapability capability) noexcept {
  switch (capability) {
    case PluginCapability::ScoreRead: return "score.read";
    case PluginCapability::SelectionRead: return "selection.read";
    case PluginCapability::AnalysisHarmony: return "analysis.harmony";
    case PluginCapability::ScoreCommandSubmit: return "score.command.submit";
    case PluginCapability::PreviewRender: return "preview.render";
    case PluginCapability::PartsCreate: return "parts.create";
    case PluginCapability::ImportFormat: return "import.format";
    case PluginCapability::ExportFormat: return "export.format";
    case PluginCapability::DockPanel: return "ui.dock";
    case PluginCapability::Inspector: return "ui.inspector";
    case PluginCapability::PlaybackProcessor: return "playback.processor";
    case PluginCapability::AlternativeNotationView: return "notation.alternative.view";
    case PluginCapability::FileSystemRead: return "filesystem.read";
    case PluginCapability::FileSystemWrite: return "filesystem.write";
    case PluginCapability::Network: return "network";
    case PluginCapability::ProcessLaunch: return "process.launch";
    case PluginCapability::DeviceAccess: return "device.access";
  }
  return "unknown";
}

PluginManifestValidation PluginManifestValidator::validate(
    const PluginManifest& manifest,
    std::string_view applicationApiVersion,
    const PluginTrustPolicy& trustPolicy) {
  PluginManifestValidation result;
  const auto add = [&result](std::string field, std::string message) {
    result.issues.push_back({std::move(field), std::move(message)});
  };

  if (manifest.manifestVersion != 1) {
    add("manifestVersion", "Unsupported manifest version.");
  }
  if (!isSafePluginId(manifest.id)) {
    add("id", "Plugin ID must be a safe immutable identifier.");
  }
  if (manifest.name.empty()) add("name", "Plugin name is required.");
  if (manifest.vendor.empty()) add("vendor", "Plugin vendor is required.");

  SemVer app;
  SemVer pluginVersion;
  SemVer minApi;
  SemVer maxApi;
  const bool appValid = parseSemVer(applicationApiVersion, &app);
  const bool pluginValid = parseSemVer(manifest.version, &pluginVersion);
  const bool minValid = parseSemVer(manifest.minimumApiVersion, &minApi);
  const bool maxValid = parseSemVer(manifest.maximumApiVersion, &maxApi);
  if (!appValid) add("applicationApiVersion", "Application API version is invalid.");
  if (!pluginValid) add("version", "Plugin version must be major.minor.patch.");
  if (!minValid) add("minimumApiVersion", "Minimum API version is invalid.");
  if (!maxValid) add("maximumApiVersion", "Maximum API version is invalid.");
  if (appValid && minValid && maxValid && (app < minApi || maxApi < app)) {
    add("apiVersion", "Plugin is incompatible with the current application API.");
  }

  if (!isRelativeSafePath(manifest.entryPoint)) {
    add("entryPoint", "Entry point must remain inside the plugin package.");
  }
  if (!isRelativeSafePath(manifest.noticesPath)) {
    add("noticesPath", "Notices path must remain inside the plugin package.");
  }

  if (manifest.packageSha256.size() != 64 ||
      !std::all_of(manifest.packageSha256.begin(), manifest.packageSha256.end(),
                   [](unsigned char ch) { return std::isxdigit(ch) != 0; })) {
    add("packageSha256", "Package SHA-256 must contain 64 hexadecimal characters.");
  }

  if (trustPolicy.requireSignature) {
    if (manifest.signature.empty() || manifest.signatureKeyId.empty()) {
      add("signature", "Signature and signer key ID are required.");
    } else if (std::find(trustPolicy.trustedSignerIds.begin(),
                         trustPolicy.trustedSignerIds.end(),
                         manifest.signatureKeyId) ==
               trustPolicy.trustedSignerIds.end()) {
      add("signatureKeyId", "Signer is not trusted.");
    }
  }

  for (const auto capability : manifest.capabilities) {
    if (std::count(manifest.capabilities.begin(), manifest.capabilities.end(),
                   capability) > 1) {
      add("capabilities",
          "Duplicate capability: " + std::string(pluginCapabilityName(capability)));
    }
  }
  if (hasCapability(manifest, PluginCapability::ScoreCommandSubmit) &&
      !hasCapability(manifest, PluginCapability::ScoreRead)) {
    add("capabilities", "score.command.submit requires score.read.");
  }
  if (hasCapability(manifest, PluginCapability::AnalysisHarmony) &&
      !hasCapability(manifest, PluginCapability::ScoreRead)) {
    add("capabilities", "analysis.harmony requires score.read.");
  }

  for (const auto& dependency : manifest.dependencies) {
    if (!isSafePluginId(dependency.id)) {
      add("dependencies", "Dependency has an invalid plugin ID.");
    }
    SemVer ignored;
    if (!parseSemVer(dependency.minimumVersion, &ignored)) {
      add("dependencies", "Dependency minimum version is invalid.");
    }
  }

  for (const auto& contribution : manifest.uiContributions) {
    if (contribution.contributionId.empty() || contribution.location.empty() ||
        contribution.commandId.empty()) {
      add("uiContributions",
          "UI contributions require contribution, location, and command IDs.");
    }
  }

  return result;
}

PluginPermissionGrant::PluginPermissionGrant(
    std::vector<PluginCapability> approvedCapabilities)
    : approvedCapabilities_(std::move(approvedCapabilities)) {}

bool PluginPermissionGrant::contains(PluginCapability capability) const noexcept {
  return std::find(approvedCapabilities_.begin(), approvedCapabilities_.end(),
                   capability) != approvedCapabilities_.end();
}

bool PluginPermissionGate::requiresPerOperationConfirmation(
    PluginOperation operation) noexcept {
  return operation == PluginOperation::SubmitScoreCommand ||
         operation == PluginOperation::CreateParts;
}

bool PluginPermissionGate::canUse(
    const PluginManifest& manifest,
    const PluginPermissionGrant& grant,
    PluginOperation operation,
    bool userConfirmed) {
  PluginCapability required = PluginCapability::ScoreRead;
  switch (operation) {
    case PluginOperation::ReadScore:
      required = PluginCapability::ScoreRead;
      break;
    case PluginOperation::ReadSelection:
      required = PluginCapability::SelectionRead;
      break;
    case PluginOperation::AnalyzeHarmony:
      required = PluginCapability::AnalysisHarmony;
      break;
    case PluginOperation::RenderPreview:
      required = PluginCapability::PreviewRender;
      break;
    case PluginOperation::SubmitScoreCommand:
      required = PluginCapability::ScoreCommandSubmit;
      break;
    case PluginOperation::CreateParts:
      required = PluginCapability::PartsCreate;
      break;
  }

  if (!hasCapability(manifest, required) || !grant.contains(required)) return false;
  if (requiresPerOperationConfirmation(operation) && !userConfirmed) return false;
  return true;
}

}  // namespace airmon
