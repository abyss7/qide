#include <ui/main_window.h>

#include "ui_main_window.h"

#include <project/ninja_project.h>
#include <ui/file_item.h>
#include <ui/folder_item.h>

#include <QFileDialog>
#include <QMessageBox>

namespace ide {
namespace ui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow) {
  ui_->setupUi(this);
  ui_->splitter->setStretchFactor(0, 1);
  ui_->splitter->setStretchFactor(1, 2);
  ui_->progressBar->setVisible(false);
}

MainWindow::~MainWindow() {
  delete ui_;
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
  auto project_path = QFileDialog::getExistingDirectory(
      this, "Open Project Folder", QDir::homePath());
  if (project_path.isEmpty()) {
    return;
  }
  project_path += QDir::separator() + QString(".qide");
  if (!QFile::exists(project_path)) {
    // TODO: should show warning box.
    return;
  }

  ui_->actionClose->activate(QAction::Trigger);
  try {
    UniquePtr<NinjaProject> new_project(
        new NinjaProject(AbsolutePath(project_path)));
    if (!new_project->switch_variant(0)) {
      throw std::runtime_error("Can't switch to default variant!");
    }

    ui_->projectTree->OpenProject(new_project.get(), ui_->progressBar);
    for (ui32 i = 0; i < new_project->variant_size(); ++i) {
      ui_->configurationBox->addItem(new_project->variant_name(i));
    }
    ui_->configurationBox->setCurrentIndex(0);
    new_project.release();

    ui_->splitter->setEnabled(true);
    ui_->comboBox->setEnabled(false);
    ui_->codeEditor->setEnabled(false);
    ui_->actionClose->setEnabled(true);
  } catch (std::exception& e) {
    QMessageBox::warning(this, "Error", e.what());
    ui_->actionClose->activate(QAction::Trigger);
  }
}

void MainWindow::SwitchVariant(int index) {
  if (index == -1) {
    return;
  }

  ui_->codeEditor->CloseFile();
  ui_->buttonRemoveFile->setEnabled(false);
  ui_->buttonSaveFile->setEnabled(false);

  ui_->projectTree->SwitchVariant(static_cast<ui32>(index), ui_->progressBar);
}

void MainWindow::CloseProject() {
  ui_->codeEditor->CloseFile();
  ui_->projectTree->CloseProject();
  ui_->splitter->setEnabled(false);
  ui_->actionClose->setEnabled(false);
  ui_->buttonRemoveFile->setEnabled(false);
  ui_->buttonSaveFile->setEnabled(false);
  ui_->configurationBox->clear();
}

void MainWindow::SelectFile(QTreeWidgetItem* item, QTreeWidgetItem*) {
  // We can't remove non-temporary files from project.
  ui_->buttonRemoveFile->setEnabled(
      item && item->type() == FileItem::Type &&
      !static_cast<FileItem*>(item)->temporary);
}

}  // namespace ui
}  // namespace ide
