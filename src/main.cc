#include <ui/main_window.h>

#include <QApplication>

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  MainWindow w;
  w.showMaximized();

  return a.exec();
}
