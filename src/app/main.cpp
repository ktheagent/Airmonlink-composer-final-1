#include "app/main_window.h"

#include <QApplication>

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("Airmonlink Composer Native");
  QApplication::setApplicationVersion("0.1.0");
  airmon::MainWindow window;
  window.show();
  return app.exec();
}
