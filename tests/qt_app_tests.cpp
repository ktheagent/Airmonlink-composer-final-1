#include "app/main_window.h"

#include <QAction>
#include <QApplication>
#include <QTest>
#include <QToolBar>

class AppShellTest final : public QObject {
  Q_OBJECT

 private slots:
  void mainWindowInitializes() {
    airmon::MainWindow window;
    window.show();

    QTRY_VERIFY_WITH_TIMEOUT(window.isVisible(), 2000);
    QVERIFY(window.centralWidget() != nullptr);
    QVERIFY(window.windowTitle().contains(QStringLiteral("Foundation 0.1.0")));

    const auto toolBars = window.findChildren<QToolBar*>();
    QCOMPARE(toolBars.size(), qsizetype(1));
    QCOMPARE(toolBars.front()->windowTitle(), QStringLiteral("Notation Input"));

    QAction* quarter = nullptr;
    for (QAction* action : toolBars.front()->actions()) {
      if (action->text() == QStringLiteral("Quarter")) {
        quarter = action;
        break;
      }
    }
    QVERIFY(quarter != nullptr);
    QTest::mouseClick(toolBars.front()->widgetForAction(quarter), Qt::LeftButton);
    QApplication::processEvents();

    window.close();
    QTRY_VERIFY_WITH_TIMEOUT(!window.isVisible(), 2000);
  }
};

QTEST_MAIN(AppShellTest)
#include "qt_app_tests.moc"
