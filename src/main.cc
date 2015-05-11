#include <ui/main_window.h>

#include <QApplication>

int main(int argc, char* argv[]) {
  QApplication application(argc, argv);
  ide::ui::MainWindow window;
  window.showMaximized();

  return application.exec();
}
