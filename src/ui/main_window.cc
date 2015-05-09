#include <ui/main_window.h>

#include <project/ninja_project.h>
#include <ui/file_tree_item.h>
#include <ui/folder_tree_item.h>

#include <QFileDialog>
#include <QMessageBox>

#include <iostream>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  ui_.setupUi(this);
  ui_.splitter->setStretchFactor(0, 1);
  ui_.splitter->setStretchFactor(1, 2);
}

void MainWindow::NewProject() {
  QString project_path = QFileDialog::getExistingDirectory(
      this, "Choose the New Project Folder", QDir::homePath());
  if (project_path.isEmpty()) {
    return;
  }
  QString project_name = QDir(project_path).dirName();
  project_path += QDir::separator() + QString(".qide");
  if (QFile::exists(project_path)) {
    // TODO: should show warning box.
    return;
  }

  // TODO: reimplement New Ninja Project.
  //  std::unique_ptr<ide::NinjaProject> new_project(
  //      ide::NinjaProject::New(project_name, project_path));

  //  ui_.actionClose->activate(QAction::Trigger);
  //  try {
  //    ui_.projectTree->OpenProject(new_project.release());
  //    ui_.splitter->setEnabled(true);
  //    ui_.comboBox->setEnabled(false);
  //    ui_.codeEditor->setEnabled(false);
  //    ui_.actionClose->setEnabled(true);
  //  } catch (std::exception& e) {
  //    QMessageBox::warning(this, "Error", e.what());
  //    ui_.actionClose->activate(QAction::Trigger);
  //  }
}

void MainWindow::OpenProject() {
  QString project_path = QFileDialog::getExistingDirectory(
      this, "Open Project Folder", QDir::homePath());
  if (project_path.isEmpty()) {
    return;
  }
  project_path += QDir::separator() + QString(".qide");
  if (!QFile::exists(project_path)) {
    // TODO: should show warning box.
    return;
  }

  ui_.actionClose->activate(QAction::Trigger);
  try {
    auto new_project = new ide::NinjaProject(project_path);
    ui_.projectTree->OpenProject(new_project);
    ui_.splitter->setEnabled(true);
    ui_.comboBox->setEnabled(false);
    ui_.codeEditor->setEnabled(false);
    ui_.actionClose->setEnabled(true);

    for (auto i = 0u; i < new_project->VariantSize(); ++i) {
      ui_.configurationBox->addItem(new_project->GetVariantName(i));
    }
    ui_.configurationBox->setCurrentIndex(0);
  } catch (std::exception& e) {
    QMessageBox::warning(this, "Error", e.what());
    ui_.actionClose->activate(QAction::Trigger);
  }
}

void MainWindow::SwitchVariant(int index) {
  if (index == -1) {
    return;
  }

  ui_.codeEditor->CloseFile();
  ui_.buttonRemoveFile->setEnabled(false);
  ui_.buttonSaveFile->setEnabled(false);

  ui_.projectTree->SwitchVariant(static_cast<unsigned>(index));
}

void MainWindow::CloseProject() {
  ui_.codeEditor->CloseFile();
  ui_.projectTree->CloseProject();
  ui_.splitter->setEnabled(false);
  ui_.actionClose->setEnabled(false);
  ui_.buttonRemoveFile->setEnabled(false);
  ui_.buttonSaveFile->setEnabled(false);
  ui_.configurationBox->clear();
}

void MainWindow::SelectFile(QTreeWidgetItem* item, QTreeWidgetItem*) {
  ui_.buttonRemoveFile->setEnabled(
      item && item->type() == FileTreeItem::Type &&
      !static_cast<FileTreeItem*>(item)->temporary);
}

void MainWindow::OpenFile(QTreeWidgetItem* item, int) {
  if (!item) {
    ui_.codeEditor->CloseFile();
    return;
  }

  if (item->type() != FileTreeItem::Type) {
    return;
  }

  ui_.codeEditor->OpenFile(static_cast<FileTreeItem*>(item));
}
