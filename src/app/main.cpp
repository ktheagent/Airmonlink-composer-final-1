#include "app/main_window.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QTimer>

#include <cstdio>

#ifndef AIRMON_APP_VERSION
#define AIRMON_APP_VERSION "0.1.0"
#endif

#ifndef AIRMON_GIT_SHA
#define AIRMON_GIT_SHA "unknown"
#endif

namespace {
void writeStartupLog(const QString& message) {
  const QString path = qVar("AIRMON_STARTUP_LOG_PATH");
  if (path.isEmpty()) {
    return;
  }
  QFile file(path);
  if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
    QTextStream(&file) << message << '\n';
  }
}
}

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("Airmonlink Composer Native");
  QApplication::setApplicationDisplayName("Airmonlink Composer Native");
  QApplication::setApplicationVersion(AIRMON_APP_VERSION);
  QApplication::setOrganizationName("Airmonlink");
  app.setProperty("airmon.commitSha", QStringLiteral(AIRMON_GIT_SHA));

  const QString identity = QApplication::applicationDisplayName() + ' ' +
      QApplication::applicationVersion() + " commit " + AIRMON_GIT_SHA;

  if (app.arguments().contains(QStringLiteral("--version"))) {
    QTextStream(stdout) << identity << '\n';
    return 0;
  }

  writeStartupLog("starting " + identity);
  airmon::MainWindow window;
  window.show();
  QTimer::singleShot(1000, []() { writeStartupLog("initialized"); });
  const int result = app.exec();
  writeStartupLog("exit " + QString::number(result));
  return result;
}
