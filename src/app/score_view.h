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
  void setDuration(Duration value) { input_.setDuration(value); }
  void setVoice(Voice value) { input_.setVoice(value); }
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
}
