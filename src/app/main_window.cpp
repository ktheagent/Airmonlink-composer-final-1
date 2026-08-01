#include "app/main_window.h"

#include "app/score_view.h"
#include "app/smufl_symbols.h"

#include <QAction>
#include <QActionGroup>
#include <QCloseEvent>
#include <QDockWidget>
#include <QColor>
#include <QKeySequence>
#include <QPainter>
#include <QPixmap>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QSettings>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

namespace airmon {
namespace {

QString durationCommandId(Duration duration) {
  switch (duration) {
    case Duration::Whole: return QStringLiteral("write.duration.whole");
    case Duration::Half: return QStringLiteral("write.duration.half");
    case Duration::Quarter: return QStringLiteral("write.duration.quarter");
    case Duration::Eighth: return QStringLiteral("write.duration.eighth");
    case Duration::Sixteenth: return QStringLiteral("write.duration.sixteenth");
    case Duration::ThirtySecond: return QStringLiteral("write.duration.thirty-second");
    case Duration::SixtyFourth: return QStringLiteral("write.duration.sixty-fourth");
  }
  return QStringLiteral("write.duration.unknown");
}

QString voiceCommandId(Voice voice) {
  return QStringLiteral("write.voice.%1").arg(static_cast<int>(voice));
}

QColor voiceColor(Voice voice) {
  switch (voice) {
    case Voice::One: return QColor(36, 112, 255);
    case Voice::Two: return QColor(0, 150, 125);
    case Voice::Three: return QColor(230, 126, 34);
    case Voice::Four: return QColor(190, 55, 190);
  }
  return QColor(Qt::black);
}

}  // namespace

MainWindow::MainWindow() {
  setObjectName(QStringLiteral("mainWindow"));
  setWindowTitle(QStringLiteral("Airmonlink Composer Native — Write workspace"));
  resize(1280, 800);

  const MusicFontLoadResult fontResult = loadBravuraMusicFont();
  musicFont_ = fontResult.font;
  setProperty("smuflFontSource", fontResult.sourcePath);
  setProperty("smuflFontLoadedFromBundle", fontResult.loadedFromBundle);

  scoreView_ = new ScoreView(this);
  scoreView_->setObjectName(QStringLiteral("scoreView"));
  setCentralWidget(scoreView_);

  writeWorkspace_ = addToolBar(QStringLiteral("Write"));
  writeWorkspace_->setObjectName(QStringLiteral("writeWorkspace"));
  writeWorkspace_->setMovable(false);
  writeWorkspace_->setFloatable(false);
  writeWorkspace_->setToolButtonStyle(Qt::ToolButtonIconOnly);
  writeWorkspace_->setIconSize(QSize(30, 30));
  writeWorkspace_->setAllowedAreas(Qt::TopToolBarArea);
  writeWorkspace_->setProperty("workspace", QStringLiteral("Write"));

  auto* durationGroup = new QActionGroup(this);
  durationGroup->setExclusive(true);
  QList<QAction*> durationActions;
  durationActions
      << addDurationCommand(durationGroup, Duration::Whole, QStringLiteral("Whole note"), QKeySequence(QStringLiteral("1")))
      << addDurationCommand(durationGroup, Duration::Half, QStringLiteral("Half note"), QKeySequence(QStringLiteral("2")))
      << addDurationCommand(durationGroup, Duration::Quarter, QStringLiteral("Quarter note"), QKeySequence(QStringLiteral("4")))
      << addDurationCommand(durationGroup, Duration::Eighth, QStringLiteral("Eighth note"), QKeySequence(QStringLiteral("8")))
      << addDurationCommand(durationGroup, Duration::Sixteenth, QStringLiteral("Sixteenth note"), QKeySequence(QStringLiteral("6")))
      << addDurationCommand(durationGroup, Duration::ThirtySecond, QStringLiteral("Thirty-second note"), QKeySequence(QStringLiteral("3")))
      << addDurationCommand(durationGroup, Duration::SixtyFourth, QStringLiteral("Sixty-fourth note"), QKeySequence(QStringLiteral("0")));
  writeWorkspace_->addWidget(createGroup(QStringLiteral("Notes"), durationActions,
                                         QStringLiteral("ribbon")));

  auto* voiceGroup = new QActionGroup(this);
  voiceGroup->setExclusive(true);
  QList<QAction*> voiceActions;
  voiceActions
      << addVoiceCommand(voiceGroup, Voice::One, QStringLiteral("Voice 1"), QKeySequence(QStringLiteral("Alt+1")))
      << addVoiceCommand(voiceGroup, Voice::Two, QStringLiteral("Voice 2"), QKeySequence(QStringLiteral("Alt+2")))
      << addVoiceCommand(voiceGroup, Voice::Three, QStringLiteral("Voice 3"), QKeySequence(QStringLiteral("Alt+3")))
      << addVoiceCommand(voiceGroup, Voice::Four, QStringLiteral("Voice 4"), QKeySequence(QStringLiteral("Alt+4")));
  writeWorkspace_->addWidget(createGroup(QStringLiteral("Voices"), voiceActions,
                                         QStringLiteral("ribbon")));

  QAction* undoAction = new QAction(QStringLiteral("Undo"), this);
  undoAction->setObjectName(QStringLiteral("command.edit.undo"));
  undoAction->setShortcut(QKeySequence::Undo);
  undoAction->setToolTip(QStringLiteral("Undo · Ctrl+Z"));
  connect(undoAction, &QAction::triggered, this, [this] { scoreView_->undo(); });
  addAction(undoAction);

  QAction* redoAction = new QAction(QStringLiteral("Redo"), this);
  redoAction->setObjectName(QStringLiteral("command.edit.redo"));
  redoAction->setShortcut(QKeySequence::Redo);
  redoAction->setToolTip(QStringLiteral("Redo · Ctrl+Y"));
  connect(redoAction, &QAction::triggered, this, [this] { scoreView_->redo(); });
  addAction(redoAction);

  notationKeypad_ = new QDockWidget(QStringLiteral("Notation keypad"), this);
  notationKeypad_->setObjectName(QStringLiteral("notationKeypad"));
  notationKeypad_->setAllowedAreas(Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
  notationKeypad_->setFeatures(QDockWidget::DockWidgetMovable |
                                QDockWidget::DockWidgetClosable);
  QWidget* keypadBody = new QWidget(notationKeypad_);
  keypadBody->setObjectName(QStringLiteral("notationKeypadBody"));
  auto* keypadLayout = new QVBoxLayout(keypadBody);
  keypadLayout->setContentsMargins(6, 6, 6, 6);
  keypadLayout->setSpacing(4);
  keypadLayout->addWidget(createGroup(QStringLiteral("Notes"), durationActions,
                                      QStringLiteral("keypad")));
  keypadLayout->addWidget(createGroup(QStringLiteral("Voices"), voiceActions,
                                      QStringLiteral("keypad")));
  keypadLayout->addStretch();
  notationKeypad_->setWidget(keypadBody);
  addDockWidget(Qt::RightDockWidgetArea, notationKeypad_);

  restoreWorkspaceState();
}

QAction* MainWindow::command(const QString& id) const {
  return findChild<QAction*>(id);
}

QAction* MainWindow::addDurationCommand(QActionGroup* group,
                                        Duration duration,
                                        const QString& name,
                                        const QKeySequence& shortcut) {
  auto* action = new QAction(smuflIcon(durationGlyph(duration), musicFont_, QSize(30, 30)),
                             name, this);
  action->setObjectName(durationCommandId(duration));
  action->setCheckable(true);
  action->setShortcut(shortcut);
  action->setShortcutContext(Qt::WindowShortcut);
  action->setToolTip(QStringLiteral("%1 · %2").arg(name, shortcut.toString()));
  action->setData(static_cast<int>(duration));
  group->addAction(action);
  addAction(action);
  connect(action, &QAction::triggered, this, [this, duration, action] {
    scoreView_->setDuration(duration);
    QSettings().setValue(QStringLiteral("Write/lastCommand"), action->objectName());
  });
  if (duration == Duration::Quarter) action->setChecked(true);
  return action;
}

QAction* MainWindow::addVoiceCommand(QActionGroup* group,
                                     Voice voice,
                                     const QString& name,
                                     const QKeySequence& shortcut) {
  auto* action = new QAction(name, this);
  action->setObjectName(voiceCommandId(voice));
  action->setCheckable(true);
  action->setShortcut(shortcut);
  action->setShortcutContext(Qt::WindowShortcut);
  action->setToolTip(QStringLiteral("%1 · %2").arg(name, shortcut.toString()));
  action->setData(static_cast<int>(voice));
  QPixmap pixmap(30, 30);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(voiceColor(voice));
  painter.drawRoundedRect(QRect(2, 2, 26, 26), 5, 5);
  painter.setPen(Qt::white);
  QFont labelFont = font();
  labelFont.setBold(true);
  painter.setFont(labelFont);
  painter.drawText(pixmap.rect(), Qt::AlignCenter,
                   QStringLiteral("V%1").arg(static_cast<int>(voice)));
  action->setIcon(QIcon(pixmap));
  group->addAction(action);
  addAction(action);
  connect(action, &QAction::triggered, this, [this, voice, action] {
    scoreView_->setVoice(voice);
    QSettings().setValue(QStringLiteral("Write/lastCommand"), action->objectName());
  });
  if (voice == Voice::One) action->setChecked(true);
  return action;
}

QWidget* MainWindow::createGroup(const QString& groupName,
                                 const QList<QAction*>& actions,
                                 const QString& surfaceName) {
  auto* groupWidget = new QWidget(this);
  groupWidget->setObjectName(QStringLiteral("%1.%2Group").arg(surfaceName, groupName));
  groupWidget->setProperty("writeGroup", groupName);
  auto* layout = new QVBoxLayout(groupWidget);
  layout->setContentsMargins(4, 2, 4, 1);
  layout->setSpacing(1);

  auto* buttons = new QWidget(groupWidget);
  auto* buttonLayout = new QHBoxLayout(buttons);
  buttonLayout->setContentsMargins(0, 0, 0, 0);
  buttonLayout->setSpacing(2);
  for (QAction* action : actions) {
    auto* button = new QToolButton(buttons);
    button->setObjectName(QStringLiteral("%1.%2").arg(surfaceName, action->objectName()));
    button->setDefaultAction(action);
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setIconSize(QSize(30, 30));
    button->setAutoRaise(true);
    button->setAccessibleName(action->text());
    button->setProperty("commandId", action->objectName());
    button->setProperty("surface", surfaceName);
    buttonLayout->addWidget(button);
  }
  layout->addWidget(buttons);

  auto* label = new QLabel(groupName, groupWidget);
  label->setObjectName(QStringLiteral("%1.%2Label").arg(surfaceName, groupName));
  label->setAlignment(Qt::AlignCenter);
  label->setProperty("secondaryGroupLabel", true);
  QFont smallFont = label->font();
  smallFont.setPointSizeF(qMax(7.0, smallFont.pointSizeF() - 1.0));
  label->setFont(smallFont);
  label->setStyleSheet(QStringLiteral("QLabel { color: palette(mid); }"));
  layout->addWidget(label);
  return groupWidget;
}

void MainWindow::restoreWorkspaceState() {
  QSettings settings;
  const QByteArray state = settings.value(QStringLiteral("Write/windowState")).toByteArray();
  if (!state.isEmpty()) restoreState(state);
  const QString lastCommand = settings.value(
      QStringLiteral("Write/lastCommand"),
      QStringLiteral("write.duration.quarter")).toString();
  if (QAction* action = command(lastCommand)) action->trigger();
}

void MainWindow::saveWorkspaceState() {
  QSettings settings;
  settings.setValue(QStringLiteral("Write/windowState"), saveState());
}

void MainWindow::closeEvent(QCloseEvent* event) {
  saveWorkspaceState();
  QMainWindow::closeEvent(event);
}

}  // namespace airmon
