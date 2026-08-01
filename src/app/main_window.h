#pragma once

#include "core/music_types.h"

#include <QFont>
#include <QKeySequence>
#include <QList>
#include <QMainWindow>
#include <QString>

class QAction;
class QActionGroup;
class QDockWidget;
class QToolBar;
class QCloseEvent;
class QWidget;

namespace airmon {

class ScoreView;

class MainWindow final : public QMainWindow {
 public:
  MainWindow();
  ~MainWindow() override = default;

  ScoreView* scoreView() const noexcept { return scoreView_; }
  QAction* command(const QString& id) const;
  QToolBar* writeWorkspace() const noexcept { return writeWorkspace_; }
  QDockWidget* notationKeypad() const noexcept { return notationKeypad_; }

 protected:
  void closeEvent(QCloseEvent* event) override;

 private:
  QAction* addDurationCommand(QActionGroup* group,
                              Duration duration,
                              const QString& name,
                              const QKeySequence& shortcut);
  QAction* addVoiceCommand(QActionGroup* group,
                           Voice voice,
                           const QString& name,
                           const QKeySequence& shortcut);
  QWidget* createGroup(const QString& groupName,
                       const QList<QAction*>& actions,
                       const QString& surfaceName);
  void restoreWorkspaceState();
  void saveWorkspaceState();

  ScoreView* scoreView_{};
  QToolBar* writeWorkspace_{};
  QDockWidget* notationKeypad_{};
  QFont musicFont_;
};

}  // namespace airmon
