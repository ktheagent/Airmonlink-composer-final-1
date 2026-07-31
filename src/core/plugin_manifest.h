#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace airmon {

enum class PluginCapability : std::uint8_t {
  ScoreRead,
  SelectionRead,
  AnalysisHarmony,
  ScoreCommandSubmit,
  PreviewRender,
  PartsCreate,
  ImportFormat,
  ExportFormat,
  DockPanel,
  Inspector,
  PlaybackProcessor,
  AlternativeNotationView,
  FileSystemRead,
  FileSystemWrite,
  Network,
  ProcessLaunch,
  DeviceAccess
};

std::string_view pluginCapabilityName(PluginCapability capability) noexcept;

struct PluginDependency {
  std::string id;
  std::string minimumVersion;

  friend bool operator==(const PluginDependency&, const PluginDependency&) = default;
};

enum class PluginUiKind : std::uint8_t {
  Command,
  DockPanel,
  Inspector,
  AlternativeNotationView
};

struct PluginUiContribution {
  PluginUiKind kind{PluginUiKind::Command};
  std::string contributionId;
  std::string location;
  std::string commandId;

  friend bool operator==(const PluginUiContribution&,
                         const PluginUiContribution&) = default;
};

struct PluginManifest {
  std::uint32_t manifestVersion{1};
  std::string id;
  std::string name;
  std::string vendor;
  std::string version;
  std::string minimumApiVersion;
  std::string maximumApiVersion;
  std::string entryPoint;
  std::vector<PluginCapability> capabilities;
  std::vector<PluginDependency> dependencies;
  std::vector<PluginUiContribution> uiContributions;
  std::string packageSha256;
  std::string signatureKeyId;
  std::string signature;
  std::string licenseSpdx;
  std::string noticesPath;
};

struct PluginTrustPolicy {
  bool requireSignature{true};
  std::vector<std::string> trustedSignerIds;
};

struct PluginManifestIssue {
  std::string field;
  std::string message;

  friend bool operator==(const PluginManifestIssue&,
                         const PluginManifestIssue&) = default;
};

struct PluginManifestValidation {
  std::vector<PluginManifestIssue> issues;

  bool ok() const noexcept { return issues.empty(); }
};

class PluginManifestValidator {
 public:
  static PluginManifestValidation validate(
      const PluginManifest& manifest,
      std::string_view applicationApiVersion,
      const PluginTrustPolicy& trustPolicy);
};

class PluginPermissionGrant {
 public:
  explicit PluginPermissionGrant(
      std::vector<PluginCapability> approvedCapabilities);

  bool contains(PluginCapability capability) const noexcept;

 private:
  std::vector<PluginCapability> approvedCapabilities_;
};

enum class PluginOperation : std::uint8_t {
  ReadScore,
  ReadSelection,
  AnalyzeHarmony,
  RenderPreview,
  SubmitScoreCommand,
  CreateParts
};

class PluginPermissionGate {
 public:
  static bool requiresPerOperationConfirmation(
      PluginOperation operation) noexcept;

  static bool canUse(const PluginManifest& manifest,
                     const PluginPermissionGrant& grant,
                     PluginOperation operation,
                     bool userConfirmed);
};

}  // namespace airmon
