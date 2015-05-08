#pragma once

#include "ui_main_window.h"

#include <QMainWindow>

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);

 public slots:
  void NewProject();
  void OpenProject();
  void SwitchConfiguration(int index);
  void CloseProject();
  void SelectFile(QTreeWidgetItem* item, int column);
  void OpenFile(QTreeWidgetItem* item, int column);

 private:
  Ui::MainWindow ui_;
};
