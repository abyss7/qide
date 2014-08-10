#include <ui/main_window.h>

#include <project/simple_project.h>
#include <ui/file_tree_item.h>
#include <ui/folder_tree_item.h>

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  ui_.setupUi(this);
  ui_.splitter->setStretchFactor(0, 1);
  ui_.splitter->setStretchFactor(1, 2);
}

void MainWindow::OpenProject() {
  QString file_path = QFileDialog::getOpenFileName(
      this, "Open Project File", QDir::homePath(), "(*.project)");
  if (file_path.isEmpty()) {
    return;
  }

  ui_.actionClose->activate(QAction::Trigger);
  try {
    ui_.projectTree->OpenProject(new ide::SimpleProject(file_path));
    ui_.splitter->setEnabled(true);
    ui_.comboBox->setEnabled(false);
    ui_.codeEditor->setEnabled(false);
    ui_.actionClose->setEnabled(true);
  } catch (std::exception& e) {
    QMessageBox::warning(this, "Error", e.what());
    ui_.actionClose->activate(QAction::Trigger);
  }
}

void MainWindow::CloseProject() {
  ui_.codeEditor->CloseFile();
  ui_.projectTree->CloseProject();
  ui_.splitter->setEnabled(false);
  ui_.actionClose->setEnabled(false);
  ui_.buttonRemoveFile->setEnabled(false);
  ui_.buttonSaveFile->setEnabled(false);
}

void MainWindow::SelectFile(QTreeWidgetItem* item, int) {
  ui_.buttonRemoveFile->setEnabled(item->type() == FileTreeItem::Type);
}

void MainWindow::OpenFile(QTreeWidgetItem* item, int) {
  if (!item || item->type() != FileTreeItem::Type) {
    return;
  }

  if (ui_.codeEditor->OpenFile(static_cast<FileTreeItem*>(item))) {
    ui_.buttonSaveFile->setEnabled(true);
  }
}
