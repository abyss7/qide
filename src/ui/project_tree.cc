#include <ui/project_tree.h>

#include <project/ninja_project.h>
#include <ui/file_item.h>
#include <ui/folder_item.h>

#include <QFileDialog>
#include <QHeaderView>

namespace ide {
namespace ui {

ProjectTree::ProjectTree(QWidget* parent) : QTreeWidget(parent) {}

ProjectTree::~ProjectTree() {
  CloseProject();
}

void ProjectTree::OpenProject(NinjaProject* project, QProgressBar* progress) {
  Q_ASSERT(project);
  Q_ASSERT(!project_);
  delete project_;
  project_ = project;
  parser_.reset(new index::ClangParser);

  Populate(progress);

  setHeaderLabel(project_->name());
  header()->show();
}

void ProjectTree::SwitchVariant(ui32 index, QProgressBar* progress) {
  if (project_->variant() == index) {
    return;
  }

  clear();

  project_->switch_variant(index);
  Populate(progress);
}

void ProjectTree::CloseProject() {
  header()->close();
  clear();
  delete project_;
  project_ = nullptr;
}

void ProjectTree::AddNewFile() {
  // TODO: implement this.
}

void ProjectTree::AddExistingFile() {
  String dialog_path = project_->root();

  if (!selectedItems().empty()) {
    const auto* item = selectedItems().front();
    if (item->type() == FileItem::Type) {
      item = item->parent();
    }
    dialog_path = static_cast<const FolderItem*>(item)->FullPath();
  }
  String file_name = QFileDialog::getOpenFileName(
      this, "Add File To Project", dialog_path, "All files (*.*)", 0,
      QFileDialog::HideNameFilterDetails);
  if (file_name.isEmpty()) {
    return;
  }

  auto it = project_->AppendFile(AbsolutePath(file_name));
  if (it != project_->end()) {
    file_name = file_name.mid(project_->root().str.size() + 1);
    ShowFile(it);
  }
}

void ProjectTree::RemoveFile() {
  Q_ASSERT(selectedItems().front()->type() == FileItem::Type);

  auto* item = selectedItems().front();
  itemActivated(nullptr, 0);
  project_->RemoveFile(static_cast<FileItem*>(item)->RelativePath());
  item->parent()->removeChild(item);
  // TODO: also remove all empty parent folders.
  delete item;
}

void ProjectTree::Populate(QProgressBar* progress) {
  progress->setMaximum(project_->size());
  progress->setValue(0);
  progress->setVisible(true);

  auto* root_item = new FolderItem(project_->root());
  addTopLevelItem(root_item);

  setSortingEnabled(false);
  for (auto it = project_->begin(), end = project_->end(); it != end; ++it) {
    ShowFile(it);
    progress->setValue(progress->value() + 1);
  }
  setSortingEnabled(true);

  // TODO: sort items.

  progress->setVisible(false);
}

void ProjectTree::ShowFile(Project::Iterator file) {
  auto path_elements = file.path().str.split(QDir::separator());
  auto file_name = path_elements.back();
  path_elements.pop_back();

  auto* folder_item = static_cast<FolderItem*>(topLevelItem(0));
  for (const auto& path : path_elements) {
    folder_item = folder_item->AddSubfolder(path);
  }
  folder_item->addChild(new FileItem(file_name, file, parser_.get()));
}

}  // namespace ui
}  // namespace ide
