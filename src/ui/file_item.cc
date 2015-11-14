#include <ui/file_item.h>

#include <QDir>

#include <iostream>

namespace ide {
namespace ui {

FileItem::FileItem(const String& name, Project::Iterator it,
                   index::ClangParser* parser)
    : QTreeWidgetItem(Type),
      temporary(it.has_args()),
      it_(it),
      parser_(parser) {
  setText(0, name);
  setToolTip(0, name);
  setIcon(0, QIcon::fromTheme("text-x-generic"));

  auto tmp_font = font(0);
  tmp_font.setUnderline(!temporary);
  setFont(0, tmp_font);
}

AbsolutePath FileItem::FullPath() const {
  String full_path = text(0);
  const QTreeWidgetItem* item = this;
  while (item->parent()) {
    item = item->parent();
    full_path = item->text(0) + QDir::separator() + full_path;
  }

  return AbsolutePath(full_path);
}

RelativePath FileItem::RelativePath() const {
  String full_path = text(0);
  const QTreeWidgetItem* item = this;
  while (item->parent()->parent()) {
    item = item->parent();
    full_path = item->text(0) + QDir::separator() + full_path;
  }

  return ide::RelativePath(full_path);
}

void FileItem::CodeComplete(ui32 line, ui32 column) {
  if (!it_.has_args()) {
    return;
  }

  parser_->Complete(FullPath(), it_.args(), it_.args_dir(), line, column);
}

bool FileItem::operator<(const QTreeWidgetItem& other) const {
  // Sort files below folders.
  if (treeWidget()->sortColumn() == 0 && other.type() != Type) {
    return false;
  }

  return QTreeWidgetItem::operator<(other);
}

}  // namespace ui
}  // namespace ide
