#include <ui/file_tree_item.h>

#include <QDir>

#include <iostream>

namespace ide {
namespace ui {

FileTreeItem::FileTreeItem(const String& name, const StringList& args,
                           bool temporary)
    : QTreeWidgetItem(Type), temporary(temporary) {
  setText(0, name);
  setToolTip(0, name);
  setIcon(0, QIcon::fromTheme("text-x-generic"));

  auto tmp_font = font(0);
  tmp_font.setUnderline(!temporary);
  setFont(0, tmp_font);

  for (ui32 i = 0, s = args.size(); i < s; ++i) {
    if (i == 0) {
      // Skip the first argument - usually it's a "clang++".
      continue;
    }
    args_.push_back(args[i].toStdString());
  }
}

AbsolutePath FileTreeItem::FullPath() const {
  String full_path = text(0);
  const QTreeWidgetItem* item = this;
  while (item->parent()) {
    item = item->parent();
    full_path = item->text(0) + QDir::separator() + full_path;
  }

  return AbsolutePath(full_path);
}

RelativePath FileTreeItem::RelativePath() const {
  String full_path = text(0);
  const QTreeWidgetItem* item = this;
  while (item->parent()->parent()) {
    item = item->parent();
    full_path = item->text(0) + QDir::separator() + full_path;
  }

  return ide::RelativePath(full_path);
}

bool FileTreeItem::operator<(const QTreeWidgetItem& other) const {
  if (treeWidget()->sortColumn() == 0 && other.type() != Type) {
    return false;
  }

  // FIXME: not reached.
  return QTreeWidgetItem::operator<(other);
}

}  // namespace ui
}  // namespace ide
