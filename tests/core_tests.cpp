#include "core/coordinate_mapper.h"
#include "core/note_input_controller.h"
#include "core/score_model.h"

#include <cstdlib>
#include <iostream>
#include <string>

using namespace airmon;

namespace {
int failures = 0;
void expect(bool condition, const std::string& message) {
  if (!condition) { std::cerr << "FAIL: " << message << '\n'; ++failures; }
}
}

int main() {
  StaffGeometry geometry;
  CoordinateMapper mapper(geometry);
  ScoreModel score;
  NoteInputController input(score, mapper);
  const ViewTransform normal{};

  const auto topLine = input.preview({geometry.staffLeft, geometry.firstSystemTop}, normal);
  expect(topLine.has_value(), "pointer over staff creates a ghost note");
  expect(topLine && topLine->target.midiPitch == 77, "top treble line maps to F5");
  expect(topLine && topLine->target.onsetBeats == 0.0, "staff start maps to first beat");
  expect(topLine && topLine->opacity > 0.3 && topLine->opacity < 0.5, "ghost note is faint");

  const auto committed = input.commit({geometry.staffLeft, geometry.firstSystemTop}, normal);
  expect(committed.has_value(), "valid ghost commits");
  expect(score.notes().size() == 1, "commit creates exactly one semantic note");
  expect(committed && committed->midiPitch == topLine->target.midiPitch, "commit pitch equals preview pitch");
  expect(committed && committed->onsetBeats == topLine->target.onsetBeats, "commit onset equals preview onset");

  const auto duplicate = input.preview({geometry.staffLeft, geometry.firstSystemTop}, normal);
  expect(duplicate && !duplicate->valid, "duplicate preview is invalid before mutation");
  expect(!input.commit({geometry.staffLeft, geometry.firstSystemTop}, normal), "invalid placement cannot commit");
  expect(score.notes().size() == 1, "rejected placement is atomic");

  expect(input.undo(), "note entry undo succeeds");
  expect(score.notes().empty(), "undo removes note");
  expect(input.redo(), "note entry redo succeeds");
  expect(score.notes().size() == 1, "redo restores note");

  ViewTransform zoomed;
  zoomed.viewportOrigin = {20, 70};
  zoomed.scrollOffset = {40, 100};
  zoomed.pageOrigin = {12, 18};
  zoomed.zoom = 1.5;
  const Point targetPage{geometry.staffLeft + 100, geometry.firstSystemTop + geometry.staffSpace};
  const Point client{
      (targetPage.x + zoomed.pageOrigin.x) * zoomed.zoom - zoomed.scrollOffset.x + zoomed.viewportOrigin.x,
      (targetPage.y + zoomed.pageOrigin.y) * zoomed.zoom - zoomed.scrollOffset.y + zoomed.viewportOrigin.y};
  const auto mapped = mapper.map(client, zoomed);
  expect(mapped.has_value(), "mapping works with zoom, page offset, ribbon/panel origin and scroll");
  expect(mapped && mapped->midiPitch == 74, "second line maps to D5 under transformed view");

  const auto firstSave = score.deterministicText();
  const auto secondSave = score.deterministicText();
  expect(firstSave == secondSave, "unchanged deterministic save is byte-identical");

  input.cancelPreview();
  expect(!input.previewVisible(), "escape/cancel removes ghost without mutation");

  if (failures == 0) std::cout << "PASS: native vertical-slice core tests\n";
  return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
