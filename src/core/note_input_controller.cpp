#include "core/note_input_controller.h"

#include <stdexcept>

namespace airmon {

NoteInputController::NoteInputController(ScoreModel& score, CoordinateMapper& mapper)
    : score_(score), mapper_(mapper) {}

std::optional<GhostNote> NoteInputController::preview(
    Point client, const ViewTransform& transform) const {
  const auto target = mapper_.map(client, transform);
  previewVisible_ = target.has_value();
  if (!target) return std::nullopt;
  std::string reason;
  const bool valid = score_.canPlace(target->midiPitch, target->onsetBeats,
                                     duration_, voice_, &reason);
  return GhostNote{*target, duration_, voice_, valid, reason, 0.42};
}

std::optional<NoteEvent> NoteInputController::commit(
    Point client, const ViewTransform& transform) {
  const auto ghost = preview(client, transform);
  if (!ghost || !ghost->valid) return std::nullopt;
  const auto& stored = score_.addNote(ghost->target.midiPitch,
                                        ghost->target.onsetBeats,
                                        ghost->duration, ghost->voice);
  undoStack_.push_back(stored);
  redoStack_.clear();
  return stored;
}

bool NoteInputController::undo() {
  if (undoStack_.empty()) return false;
  const NoteEvent note = undoStack_.back();
  if (!score_.removeNote(note.id)) return false;
  undoStack_.pop_back();
  redoStack_.push_back(note);
  return true;
}

bool NoteInputController::redo() {
  if (redoStack_.empty()) return false;
  const NoteEvent note = redoStack_.back();

  try {
    const auto& restored = score_.restoreNote(note);
    redoStack_.pop_back();
    undoStack_.push_back(restored);
    return true;
  } catch (const std::invalid_argument&) {
    return false;
  }
}

}  // namespace airmon
