#pragma once

#include <base/alias.h>

#include <QTreeWidgetItem>

namespace ide {
namespace ui {

class FolderTreeItem : public QTreeWidgetItem {
 public:
  enum { Type = UserType + 1 };

  explicit FolderTreeItem(const String& path);

  FolderTreeItem* AddSubfolder(const String& name);
  String FullPath() const;

  bool operator<(const QTreeWidgetItem& other) const override;

 private:
  HashMap<String, FolderTreeItem*> subfolders_;
};

}  // namespace ui
}  // namespace ide
