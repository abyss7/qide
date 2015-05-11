#include <ui/folder_tree_item.h>

#include <QDir>

namespace ide {
namespace ui {

FolderTreeItem::FolderTreeItem(const String& path) : QTreeWidgetItem(Type) {
  setText(0, path);
  setIcon(0, QIcon::fromTheme("folder"));
}

FolderTreeItem* FolderTreeItem::AddSubfolder(const String& name) {
  auto it = subfolders_.find(name);
  if (it != subfolders_.end()) {
    return it.value();
  }
  auto* item = new FolderTreeItem(name);
  subfolders_.insert(name, item);
  addChild(item);

  return item;
}

String FolderTreeItem::FullPath() const {
  String full_path = text(0);
  const QTreeWidgetItem* item = this;
  while (item->parent()) {
    item = item->parent();
    full_path = item->text(0) + QDir::separator() + full_path;
  }

  return full_path;
}

bool FolderTreeItem::operator<(const QTreeWidgetItem& other) const {
  if (treeWidget()->sortColumn() == 0 && other.type() != Type) {
    return true;
  }

  // FIXME: not reached.
  return QTreeWidgetItem::operator<(other);
}

}  // namespace ui
}  // namespace ide
