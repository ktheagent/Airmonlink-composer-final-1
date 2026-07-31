#pragma once

#include <QMainWindow>

namespace airmon {
class ScoreView;
class MainWindow final : public QMainWindow {
 public:
  MainWindow();
 private:
  ScoreView* scoreView_{};
};
}
