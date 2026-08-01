#pragma once

#include "core/coordinate_mapper.h"
#include "core/score_model.h"

#include <optional>
#include <vector>

namespace airmon {

struct GhostNote {
  PointerTarget target;
  Duration duration;
  Voice voice;
  bool valid;
  std::string message;
  double opacity{0.42};
};

class NoteInputController {
 public:
  NoteInputController(ScoreModel& score, CoordinateMapper& mapper);

  void setDuration(Duration value) noexcept { duration_ = value; }
  void setVoice(Voice value) noexcept { voice_ = value; }
  Duration duration() const noexcept { return duration_; }
  Voice voice() const noexcept { return voice_; }

  std::optional<GhostNote> preview(
      Point client,
      const ViewTransform& transform) const;
  std::optional<NoteEvent> commit(
      Point client,
      const ViewTransform& transform);
  bool undo();
  bool redo();
  void cancelPreview() noexcept { previewVisible_ = false; }
  bool previewVisible() const noexcept { return previewVisible_; }

 private:
  ScoreModel& score_;
  CoordinateMapper& mapper_;
  Duration duration_{Duration::Quarter};
  Voice voice_{Voice::One};
  mutable bool previewVisible_{false};
  std::vector<NoteEvent> undoStack_;
  std::vector<NoteEvent> redoStack_;
};

}  // namespace airmon
