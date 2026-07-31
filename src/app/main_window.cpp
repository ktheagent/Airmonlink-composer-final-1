#include "app/main_window.h"
#include "app/score_view.h"

#include <QAction>
#include <QToolBar>

namespace airmon {
MainWindow::MainWindow() {
  setWindowTitle("Airmonlink Composer Native — Foundation 0.1.0");
  resize(1280, 800);
  scoreView_ = new ScoreView(this);
  setCentralWidget(scoreView_);

  auto* notation = addToolBar("Notation Input");
  notation->setMovable(true);
  notation->addAction("Whole", this, [this] { scoreView_->setDuration(Duration::Whole); });
  notation->addAction("Half", this, [this] { scoreView_->setDuration(Duration::Half); });
  notation->addAction("Quarter", this, [this] { scoreView_->setDuration(Duration::Quarter); });
  notation->addAction("Eighth", this, [this] { scoreView_->setDuration(Duration::Eighth); });
  notation->addSeparator();
  notation->addAction("V1", this, [this] { scoreView_->setVoice(Voice::One); });
  notation->addAction("V2", this, [this] { scoreView_->setVoice(Voice::Two); });
  notation->addAction("V3", this, [this] { scoreView_->setVoice(Voice::Three); });
  notation->addAction("V4", this, [this] { scoreView_->setVoice(Voice::Four); });
}
}
