#include <ui/file_tree_item.h>

#include <QDir>

FileTreeItem::FileTreeItem(const QString& name) : QTreeWidgetItem(Type) {
  setText(0, name);
  setIcon(0, QIcon::fromTheme("text-x-generic"));
}

QString FileTreeItem::FullPath() const {
  QString full_path = text(0);
  const QTreeWidgetItem* item = this;
  while (item->parent()) {
    item = item->parent();
    full_path = item->text(0) + QDir::separator() + full_path;
  }

  return full_path;
}

bool FileTreeItem::operator<(const QTreeWidgetItem& other) const {
  if (treeWidget()->sortColumn() == 0 && other.type() != Type) {
    return false;
  }

  // FIXME: not reached.
  return QTreeWidgetItem::operator<(other);
}
