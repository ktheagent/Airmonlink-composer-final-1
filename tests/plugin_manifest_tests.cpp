#include "core/plugin_manifest.h"

#include <cstdlib>
#include <iostream>
#include <string>

using namespace airmon;

namespace {
int failures = 0;

void expect(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    ++failures;
  }
}

PluginManifest validManifest() {
  PluginManifest manifest;
  manifest.id = "com.airmonlink.fixture.harmony";
  manifest.name = "Harmless Harmony Fixture";
  manifest.vendor = "Airmonlink Tests";
  manifest.version = "1.0.0";
  manifest.minimumApiVersion = "1.0.0";
  manifest.maximumApiVersion = "1.0.0";
  manifest.entryPoint = "bin/fixture.dll";
  manifest.capabilities = {
      PluginCapability::ScoreRead,
      PluginCapability::SelectionRead,
      PluginCapability::AnalysisHarmony,
      PluginCapability::PreviewRender,
      PluginCapability::ScoreCommandSubmit};
  manifest.packageSha256 = std::string(64, 'a');
  manifest.signatureKeyId = "fixture-test-key";
  manifest.signature = "fixture-signature";
  manifest.licenseSpdx = "MIT";
  manifest.noticesPath = "THIRD_PARTY_NOTICES.md";
  return manifest;
}
}  // namespace

int main() {
  const PluginTrustPolicy trust{true, {"fixture-test-key"}};
  const auto valid = validManifest();

  expect(PluginManifestValidator::validate(valid, "1.0.0", trust).ok(),
         "valid signed fixture manifest passes");

  auto incompatible = valid;
  incompatible.minimumApiVersion = "2.0.0";
  expect(!PluginManifestValidator::validate(incompatible, "1.0.0", trust).ok(),
         "incompatible API is rejected");

  auto corrupt = valid;
  corrupt.packageSha256 = "not-a-sha";
  expect(!PluginManifestValidator::validate(corrupt, "1.0.0", trust).ok(),
         "invalid package hash is rejected");

  auto traversal = valid;
  traversal.entryPoint = "../escape.dll";
  expect(!PluginManifestValidator::validate(traversal, "1.0.0", trust).ok(),
         "package path traversal is rejected");

  auto untrusted = valid;
  untrusted.signatureKeyId = "unknown-key";
  expect(!PluginManifestValidator::validate(untrusted, "1.0.0", trust).ok(),
         "untrusted signer is rejected");

  auto undeclaredDependency = valid;
  undeclaredDependency.capabilities = {PluginCapability::ScoreCommandSubmit};
  expect(!PluginManifestValidator::validate(
              undeclaredDependency, "1.0.0", trust).ok(),
         "score command submission without score.read is rejected");

  const PluginPermissionGrant readAndPreview(
      std::vector<PluginCapability>{
          PluginCapability::ScoreRead, PluginCapability::SelectionRead,
          PluginCapability::AnalysisHarmony, PluginCapability::PreviewRender});

  expect(PluginPermissionGate::canUse(
             valid, readAndPreview, PluginOperation::RenderPreview, false),
         "preview works without score mutation confirmation");
  expect(!PluginPermissionGate::canUse(
             valid, readAndPreview, PluginOperation::SubmitScoreCommand, true),
         "installation permission denial blocks score mutation");

  const PluginPermissionGrant fullGrant(
      std::vector<PluginCapability>{
          PluginCapability::ScoreRead, PluginCapability::SelectionRead,
          PluginCapability::AnalysisHarmony, PluginCapability::PreviewRender,
          PluginCapability::ScoreCommandSubmit});

  expect(!PluginPermissionGate::canUse(
             valid, fullGrant, PluginOperation::SubmitScoreCommand, false),
         "score mutation requires per-operation confirmation");
  expect(PluginPermissionGate::canUse(
             valid, fullGrant, PluginOperation::SubmitScoreCommand, true),
         "confirmed score mutation is allowed through the gate");

  expect(pluginCapabilityName(PluginCapability::AnalysisHarmony) ==
             "analysis.harmony",
         "future harmony capability has the documented stable name");

  if (failures == 0) {
    std::cout << "PASS: plugin manifest and permission fixture tests\n";
  }
  return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
