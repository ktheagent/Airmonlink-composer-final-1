#pragma once

#include "core/coordinate_mapper.h"
#include "core/note_input_controller.h"
#include "core/score_model.h"

#include <QWidget>

class QColor;

namespace airmon {
class ScoreView final : public QWidget {
 public:
  explicit ScoreView(QWidget* parent = nullptr);

  void setDuration(Duration value) {
    input_.setDuration(value);
    update();
  }
  void setVoice(Voice value) {
    input_.setVoice(value);
    update();
  }
  Duration duration() const noexcept { return input_.duration(); }
  Voice voice() const noexcept { return input_.voice(); }
  bool undo() {
    const bool changed = input_.undo();
    if (changed) update();
    return changed;
  }
  bool redo() {
    const bool changed = input_.redo();
    if (changed) update();
    return changed;
  }
  const ScoreModel& scoreModel() const noexcept { return score_; }

 protected:
  void paintEvent(QPaintEvent*) override;
  void mouseMoveEvent(QMouseEvent*) override;
  void mousePressEvent(QMouseEvent*) override;
  void leaveEvent(QEvent*) override;

 private:
  QColor voiceColor(Voice voice, int alpha = 255) const;
  ScoreModel score_;
  StaffGeometry geometry_;
  CoordinateMapper mapper_;
  NoteInputController input_;
  std::optional<GhostNote> ghost_;
};
}  // namespace airmon
