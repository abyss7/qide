#include <ui/file_tree_item.h>

#include <QDir>

FileTreeItem::FileTreeItem(const QString& name, bool temporary)
    : QTreeWidgetItem(Type), temporary(temporary) {
  setText(0, name);
  setToolTip(0, name);
  setIcon(0, QIcon::fromTheme("text-x-generic"));

  auto tmp_font = font(0);
  tmp_font.setUnderline(!temporary);
  setFont(0, tmp_font);
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

QString FileTreeItem::RelativePath() const {
  QString full_path = text(0);
  const QTreeWidgetItem* item = this;
  while (item->parent()->parent()) {
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
