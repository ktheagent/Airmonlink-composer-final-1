#include "app/main_window.h"
#include "app/score_view.h"

#include <QAction>
#include <QApplication>
#include <QDockWidget>
#include <QLabel>
#include <QSettings>
#include <QTest>
#include <QToolBar>
#include <QToolButton>

class WriteWorkspaceTest final : public QObject {
  Q_OBJECT

 private:
  static QToolButton* buttonFor(airmon::MainWindow& window,
                                const QString& commandId,
                                const QString& surface) {
    const auto buttons = window.findChildren<QToolButton*>();
    for (QToolButton* button : buttons) {
      if (button->property("commandId").toString() == commandId &&
          button->property("surface").toString() == surface) {
        return button;
      }
    }
    return nullptr;
  }

 private slots:
  void initTestCase() {
    QCoreApplication::setOrganizationName(QStringLiteral("AirmonlinkTests"));
    QCoreApplication::setApplicationName(QStringLiteral("WriteWorkspaceTests"));
    QSettings().clear();
  }

  void visibleOrganizationAndSymbols() {
    airmon::MainWindow window;
    window.show();
    QCoreApplication::processEvents();

    QToolBar* workspace = window.writeWorkspace();
    QVERIFY(workspace);
    QCOMPARE(workspace->objectName(), QStringLiteral("writeWorkspace"));
    QCOMPARE(workspace->toolButtonStyle(), Qt::ToolButtonIconOnly);
    QCOMPARE(window.findChildren<QToolBar*>().size(), qsizetype(1));

    const QStringList essential{
        QStringLiteral("write.duration.whole"),
        QStringLiteral("write.duration.half"),
        QStringLiteral("write.duration.quarter"),
        QStringLiteral("write.duration.eighth"),
        QStringLiteral("write.duration.sixteenth"),
        QStringLiteral("write.duration.thirty-second"),
        QStringLiteral("write.duration.sixty-fourth"),
        QStringLiteral("write.voice.1"),
        QStringLiteral("write.voice.2"),
        QStringLiteral("write.voice.3"),
        QStringLiteral("write.voice.4")};

    for (const QString& commandId : essential) {
      QAction* action = window.command(commandId);
      QVERIFY2(action, qPrintable(commandId));
      QVERIFY(!action->toolTip().isEmpty());
      QVERIFY(!action->shortcut().isEmpty());

      QToolButton* ribbonButton = buttonFor(window, commandId, QStringLiteral("ribbon"));
      QVERIFY2(ribbonButton, qPrintable(commandId));
      QCOMPARE(ribbonButton->toolButtonStyle(), Qt::ToolButtonIconOnly);
      QVERIFY(!ribbonButton->icon().isNull());
      QVERIFY(!ribbonButton->accessibleName().isEmpty());
      QCOMPARE(ribbonButton->defaultAction(), action);

      QToolButton* keypadButton = buttonFor(window, commandId, QStringLiteral("keypad"));
      QVERIFY2(keypadButton, qPrintable(commandId));
      QCOMPARE(keypadButton->defaultAction(), action);
    }

    const auto groupLabels = window.findChildren<QLabel*>();
    int secondaryLabels = 0;
    for (const QLabel* label : groupLabels) {
      if (label->property("secondaryGroupLabel").toBool()) {
        ++secondaryLabels;
        QVERIFY(label->text() == QStringLiteral("Notes") ||
                label->text() == QStringLiteral("Voices"));
      }
    }
    QCOMPARE(secondaryLabels, 4);
  }

  void ribbonKeypadAndShortcutShareCommands() {
    airmon::MainWindow window;
    window.show();
    QCoreApplication::processEvents();

    auto* score = window.scoreView();
    QVERIFY(score);

    QToolButton* halfRibbon =
        buttonFor(window, QStringLiteral("write.duration.half"), QStringLiteral("ribbon"));
    QVERIFY(halfRibbon);
    QTest::mouseClick(halfRibbon, Qt::LeftButton);
    QCOMPARE(static_cast<int>(score->duration()), static_cast<int>(airmon::Duration::Half));

    QToolButton* eighthKeypad =
        buttonFor(window, QStringLiteral("write.duration.eighth"), QStringLiteral("keypad"));
    QVERIFY(eighthKeypad);
    QTest::mouseClick(eighthKeypad, Qt::LeftButton);
    QCOMPARE(static_cast<int>(score->duration()), static_cast<int>(airmon::Duration::Eighth));

    score->setFocus();
    QTest::keyClick(&window, Qt::Key_4);
    QCOMPARE(static_cast<int>(score->duration()), static_cast<int>(airmon::Duration::Quarter));

    QToolButton* voiceThreeRibbon =
        buttonFor(window, QStringLiteral("write.voice.3"), QStringLiteral("ribbon"));
    QVERIFY(voiceThreeRibbon);
    QTest::mouseClick(voiceThreeRibbon, Qt::LeftButton);
    QCOMPARE(static_cast<int>(score->voice()), static_cast<int>(airmon::Voice::Three));

    QTest::keyClick(&window, Qt::Key_2, Qt::AltModifier);
    QCOMPARE(static_cast<int>(score->voice()), static_cast<int>(airmon::Voice::Two));
  }

  void keypadDoesNotCoverScore() {
    airmon::MainWindow window;
    window.resize(1280, 800);
    window.show();
    QCoreApplication::processEvents();

    QDockWidget* keypad = window.notationKeypad();
    QVERIFY(keypad);
    QVERIFY(!keypad->isFloating());
    QVERIFY(window.dockWidgetArea(keypad) == Qt::RightDockWidgetArea ||
            window.dockWidgetArea(keypad) == Qt::BottomDockWidgetArea);

    const QRect scoreGlobal(window.scoreView()->mapToGlobal(QPoint(0, 0)),
                            window.scoreView()->size());
    const QRect keypadGlobal(keypad->mapToGlobal(QPoint(0, 0)), keypad->size());
    QVERIFY(!scoreGlobal.intersects(keypadGlobal));
    QVERIFY(window.writeWorkspace()->width() <= window.width());
  }

  void workspaceStatePersists() {
    {
      airmon::MainWindow window;
      window.show();
      window.command(QStringLiteral("write.duration.sixteenth"))->trigger();
      window.addDockWidget(Qt::BottomDockWidgetArea, window.notationKeypad());
      window.close();
    }

    airmon::MainWindow restored;
    QCOMPARE(static_cast<int>(restored.scoreView()->duration()), static_cast<int>(airmon::Duration::Sixteenth));
    QCOMPARE(restored.dockWidgetArea(restored.notationKeypad()), Qt::BottomDockWidgetArea);
  }
};

QTEST_MAIN(WriteWorkspaceTest)
#include "qt_app_tests.moc"
