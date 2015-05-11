#include <ui/project_tree.h>

#include <ui/file_tree_item.h>
#include <ui/folder_tree_item.h>

#include <QFileDialog>
#include <QHeaderView>

namespace ide {
namespace ui {

ProjectTree::ProjectTree(QWidget* parent) : QTreeWidget(parent) {
}

void ProjectTree::OpenProject(NinjaProject* project) {
  Q_ASSERT(project);
  Q_ASSERT(!project_);

  project_.reset(project);

  auto* root_item = new FolderTreeItem(project_->GetRoot());
  setHeaderLabel(project_->GetName());
  header()->show();
  addTopLevelItem(root_item);
  sortByColumn(0, Qt::AscendingOrder);

  setSortingEnabled(false);
  for (auto it = project_->begin(), end = project_->end(); it != end; ++it) {
    ShowFile(it);
  }
  setSortingEnabled(true);
}

void ProjectTree::SwitchVariant(ui32 index) {
  clear();

  project_->SwitchVariant(index);

  auto* root_item = new FolderTreeItem(project_->GetRoot());
  addTopLevelItem(root_item);
  sortByColumn(0, Qt::AscendingOrder);

  setSortingEnabled(false);
  for (auto it = project_->begin(), end = project_->end(); it != end; ++it) {
    ShowFile(it);
  }
  setSortingEnabled(true);
}

void ProjectTree::CloseProject() {
  header()->close();
  clear();
  project_.reset();
}

void ProjectTree::AddNewFile() {
  // TODO: implement this.
}

void ProjectTree::AddExistingFile() {
  String dialog_path = project_->GetRoot();

  if (!selectedItems().empty()) {
    const auto* item = selectedItems().front();
    if (item->type() == FileTreeItem::Type) {
      item = item->parent();
    }
    dialog_path = static_cast<const FolderTreeItem*>(item)->FullPath();
  }
  String file_name = QFileDialog::getOpenFileName(
      this, "Add File To Project", dialog_path, "All files (*.*)", 0,
      QFileDialog::HideNameFilterDetails);
  if (file_name.isEmpty()) {
    return;
  }

  auto it = project_->AddFile(file_name);
  if (it != project_->end()) {
    file_name = file_name.mid(project_->GetRoot().size() + 1);
    ShowFile(it);
  }
}

void ProjectTree::RemoveFile() {
  Q_ASSERT(selectedItems().front()->type() == FileTreeItem::Type);

  auto* item = selectedItems().front();
  itemActivated(nullptr, 0);
  project_->RemoveFile(static_cast<FileTreeItem*>(item)->RelativePath());
  item->parent()->removeChild(item);
  // TODO: also remove all empty parent folders.
  delete item;
}

void ProjectTree::ShowFile(NinjaProject::Iterator file) {
  const auto& file_path = *file;

  Q_ASSERT(!QDir::isAbsolutePath(file_path));

  auto path_elements = file_path.split(QDir::separator());
  auto file_name = path_elements.back();
  path_elements.pop_back();

  auto* folder_item = static_cast<FolderTreeItem*>(topLevelItem(0));
  for (const auto& path : path_elements) {
    folder_item = folder_item->AddSubfolder(path);
  }

  folder_item->addChild(
      new FileTreeItem(file_name, file.GetCommand(), file.IsTemporary()));
}

}  // namespace ui
}  // namespace ide
