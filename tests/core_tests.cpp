#include "core/coordinate_mapper.h"
#include "core/note_input_controller.h"
#include "core/score_model.h"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <stdexcept>
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
}  // namespace

int main() {
  expect(Rational(2, 4) == Rational(1, 2),
         "rational values normalize exactly");
  expect(Rational(1, 3) + Rational(1, 6) == Rational(1, 2),
         "rational addition is exact");
  expect(beats(Duration::Sixteenth) == Rational(1, 4),
         "sixteenth duration is one quarter beat");

  bool overflowRejected = false;
  try {
    const auto ignored =
        Rational(std::numeric_limits<std::int64_t>::max()) + Rational(1);
    (void)ignored;
  } catch (const std::overflow_error&) {
    overflowRejected = true;
  }
  expect(overflowRejected, "rational overflow is rejected");

  StaffGeometry geometry;
  CoordinateMapper mapper(geometry);
  ScoreModel score;
  NoteInputController input(score, mapper);
  const ViewTransform normal{};

  const auto topLine = input.preview(
      {geometry.staffLeft, geometry.firstSystemTop}, normal);
  expect(topLine.has_value(),
         "pointer over staff creates a ghost note");
  expect(topLine && topLine->target.midiPitch == 77,
         "top treble line maps to F5");
  expect(topLine && topLine->target.onsetBeats == Rational(0),
         "staff start maps to exact first beat");
  expect(topLine && topLine->opacity > 0.3 && topLine->opacity < 0.5,
         "ghost note is faint");

  const auto committed = input.commit(
      {geometry.staffLeft, geometry.firstSystemTop}, normal);
  expect(committed.has_value(), "valid ghost commits");
  expect(score.notes().size() == 1,
         "commit creates exactly one semantic note");
  expect(committed && topLine &&
             committed->midiPitch == topLine->target.midiPitch,
         "commit pitch equals preview pitch");
  expect(committed && topLine &&
             committed->onsetBeats == topLine->target.onsetBeats,
         "commit onset equals preview onset");

  const std::string originalId = committed ? committed->id : std::string{};
  const std::string originalSerialized = score.deterministicText();

  const auto duplicate = input.preview(
      {geometry.staffLeft, geometry.firstSystemTop}, normal);
  expect(duplicate && !duplicate->valid,
         "duplicate preview is invalid before mutation");
  expect(!input.commit(
             {geometry.staffLeft, geometry.firstSystemTop}, normal),
         "invalid placement cannot commit");
  expect(score.notes().size() == 1,
         "rejected placement is atomic");

  expect(input.undo(), "note entry undo succeeds");
  expect(score.notes().empty(), "undo removes note");
  expect(input.redo(), "note entry redo succeeds");
  expect(score.notes().size() == 1, "redo restores note");
  expect(score.notes().front().id == originalId,
         "redo preserves the original semantic note ID");
  expect(score.deterministicText() == originalSerialized,
         "undo and redo restore byte-identical serialized state");

  bool duplicateIdRejected = false;
  try {
    score.restoreNote(score.notes().front());
  } catch (const std::invalid_argument&) {
    duplicateIdRejected = true;
  }
  expect(duplicateIdRejected,
         "restoring an existing semantic ID is rejected");
  expect(score.notes().size() == 1,
         "duplicate-ID restore failure is atomic");

  bool rejectedMixedDuration = false;
  try {
    score.addNote(81, Rational(0), Duration::Half, Voice::One);
  } catch (const std::invalid_argument&) {
    rejectedMixedDuration = true;
  }
  expect(rejectedMixedDuration,
         "one chord cannot contain mixed durations");
  expect(score.notes().size() == 1,
         "failed chord insertion does not mutate score");

  ViewTransform zoomed;
  zoomed.viewportOrigin = {20, 70};
  zoomed.scrollOffset = {40, 100};
  zoomed.pageOrigin = {12, 18};
  zoomed.zoom = 1.5;
  const Point targetPage{
      geometry.staffLeft + 100,
      geometry.firstSystemTop + geometry.staffSpace};
  const Point client{
      (targetPage.x + zoomed.pageOrigin.x) * zoomed.zoom -
          zoomed.scrollOffset.x + zoomed.viewportOrigin.x,
      (targetPage.y + zoomed.pageOrigin.y) * zoomed.zoom -
          zoomed.scrollOffset.y + zoomed.viewportOrigin.y};
  const auto mapped = mapper.map(client, zoomed);
  expect(mapped.has_value(),
         "mapping works with zoom, page offset, viewport origin and scroll");
  expect(mapped && mapped->midiPitch == 74,
         "second line maps to D5 under transformed view");

  const auto firstSave = score.deterministicText();
  const auto secondSave = score.deterministicText();
  expect(firstSave == secondSave,
         "unchanged deterministic save is byte-identical");
  expect(firstSave.find(",0/1,") != std::string::npos,
         "serialization preserves exact rational onset");

  input.cancelPreview();
  expect(!input.previewVisible(),
         "cancel removes ghost without mutation");

  if (failures == 0) {
    std::cout
        << "PASS: exact-time stable-identity native vertical-slice core tests\n";
  }
  return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
