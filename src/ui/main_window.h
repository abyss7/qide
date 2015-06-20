#pragma once

#include <QMainWindow>
#include <QTreeWidgetItem>

namespace ide {
namespace ui {

namespace Ui {
class MainWindow;
}  // namespace Ui

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

 public slots:
  void NewProject();
  void OpenProject();
  void SwitchVariant(int index);
  void CloseProject();
  void SelectFile(QTreeWidgetItem* item, QTreeWidgetItem* previous_item);

 private:
  Ui::MainWindow* ui_;
};

}  // namespace ui
}  // namespace ide
