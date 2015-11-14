#include <ui/folder_item.h>

#include <QDir>

namespace ide {
namespace ui {

FolderItem::FolderItem(const String& path) : QTreeWidgetItem(Type) {
  setText(0, path);
  setIcon(0, QIcon::fromTheme("folder"));
}

FolderItem* FolderItem::AddSubfolder(const String& name) {
  auto it = subfolders_.find(name);
  if (it != subfolders_.end()) {
    return it.value();
  }
  auto* item = new FolderItem(name);
  subfolders_.insert(name, item);
  addChild(item);

  return item;
}

AbsolutePath FolderItem::FullPath() const {
  String full_path = text(0);
  const QTreeWidgetItem* item = this;
  while (item->parent()) {
    item = item->parent();
    full_path = item->text(0) + QDir::separator() + full_path;
  }

  return AbsolutePath(full_path);
}

bool FolderItem::operator<(const QTreeWidgetItem& other) const {
  // Sort folders above files.
  if (treeWidget()->sortColumn() == 0 && other.type() != Type) {
    return true;
  }

  return QTreeWidgetItem::operator<(other);
}

}  // namespace ui
}  // namespace ide
