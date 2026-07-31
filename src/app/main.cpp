#include "app/main_window.h"

#include <QApplication>
#include <QTextStream>

#include <cstdio>

#ifndef AIRMON_APP_VERSION
#define AIRMON_APP_VERSION "0.1.0"
#endif

#ifndef AIRMON_GIT_SHA
#define AIRMON_GIT_SHA "unknown"
#endif

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("Airmonlink Composer Native");
  QApplication::setApplicationDisplayName("Airmonlink Composer Native");
  QApplication::setApplicationVersion(AIRMON_APP_VERSION);
  QApplication::setOrganizationName("Airmonlink");
  app.setProperty("airmon.commitSha", QStringLiteral(AIRMON_GIT_SHA));

  if (app.arguments().contains(QStringLiteral("--version"))) {
    QTextStream out(stdout);
    out << QApplication::applicationDisplayName() << ' '
        << QApplication::applicationVersion() << " commit " << AIRMON_GIT_SHA << '\n';
    return 0;
  }

  airmon::MainWindow window;
  window.show();
  return app.exec();
}
