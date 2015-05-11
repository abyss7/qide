#pragma once

#include "ui_main_window.h"

#include <QMainWindow>

namespace ide {
namespace ui {

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);

 public slots:
  void NewProject();
  void OpenProject();
  void SwitchVariant(int index);
  void CloseProject();
  void SelectFile(QTreeWidgetItem* item, QTreeWidgetItem* previous_item);

 private:
  Ui::MainWindow ui_;
};

}  // namespace ui
}  // namespace ide
