#include "app/score_view.h"

#include <QMouseEvent>
#include <QPainter>

namespace airmon {
ScoreView::ScoreView(QWidget* parent)
    : QWidget(parent), geometry_{}, mapper_(geometry_), input_(score_, mapper_) {
  setMouseTracking(true);
  setMinimumSize(900, 700);
}

QColor ScoreView::voiceColor(Voice voice, int alpha) const {
  switch (voice) {
    case Voice::One: return QColor(36, 112, 255, alpha);
    case Voice::Two: return QColor(0, 150, 125, alpha);
    case Voice::Three: return QColor(230, 126, 34, alpha);
    case Voice::Four: return QColor(190, 55, 190, alpha);
  }
  return QColor(0, 0, 0, alpha);
}

void ScoreView::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.fillRect(rect(), QColor(25, 29, 40));
  p.fillRect(QRectF(geometry_.pageRect.x, geometry_.pageRect.y, geometry_.pageRect.width, geometry_.pageRect.height), Qt::white);
  p.setRenderHint(QPainter::Antialiasing);
  p.setPen(QPen(QColor(28, 28, 30), 1.0));
  for (std::size_t system = 0; system < geometry_.systems; ++system) {
    const double top = geometry_.firstSystemTop + system * geometry_.systemGap;
    for (int line = 0; line < 5; ++line)
      p.drawLine(QPointF(geometry_.staffLeft, top + line * geometry_.staffSpace),
                 QPointF(geometry_.staffRight, top + line * geometry_.staffSpace));
  }
  auto drawNote = [&](double x, double y, QColor color) {
    p.save(); p.setPen(Qt::NoPen); p.setBrush(color); p.translate(x, y); p.rotate(-18);
    p.drawEllipse(QRectF(-6, -4, 12, 8)); p.restore();
    p.setPen(QPen(color, 1.4)); p.drawLine(QPointF(x + 5, y), QPointF(x + 5, y - 34));
  };
  auto diatonicStepsBelowF5 = [](int midi) {
    static constexpr int degreeForPitchClass[12] = {0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6};
    const int octave = midi / 12 - 1;
    const int pitchClass = ((midi % 12) + 12) % 12;
    const int absoluteDegree = octave * 7 + degreeForPitchClass[pitchClass];
    const int f5Degree = 5 * 7 + 3;
    return f5Degree - absoluteDegree;
  };
  for (const auto& note : score_.notes()) {
    const double measureWidth = (geometry_.staffRight - geometry_.staffLeft) / geometry_.measuresPerSystem;
    const auto measure = static_cast<std::size_t>(note.onsetBeats / geometry_.beatsPerMeasure);
    const auto system = measure / geometry_.measuresPerSystem;
    const auto localMeasure = measure % geometry_.measuresPerSystem;
    const double beat = note.onsetBeats - measure * geometry_.beatsPerMeasure;
    const double x = geometry_.staffLeft + localMeasure * measureWidth + beat / geometry_.beatsPerMeasure * measureWidth;
    const double y = geometry_.firstSystemTop + system * geometry_.systemGap +
                     diatonicStepsBelowF5(note.midiPitch) * geometry_.staffSpace / 2.0;
    drawNote(x, y, voiceColor(note.voice));
  }
  if (ghost_) drawNote(ghost_->target.snappedPagePoint.x, ghost_->target.snappedPagePoint.y,
                       ghost_->valid ? voiceColor(ghost_->voice, 105) : QColor(210, 55, 55, 120));
}

void ScoreView::mouseMoveEvent(QMouseEvent* event) {
  ghost_ = input_.preview({event->position().x(), event->position().y()}, {});
  update();
}
void ScoreView::mousePressEvent(QMouseEvent* event) {
  input_.commit({event->position().x(), event->position().y()}, {});
  ghost_ = input_.preview({event->position().x(), event->position().y()}, {});
  update();
}
void ScoreView::leaveEvent(QEvent*) { ghost_.reset(); input_.cancelPreview(); update(); }
}
