#pragma once

#include <base/path.h>

#include <QTreeWidgetItem>

namespace ide {
namespace ui {

class FolderItem : public QTreeWidgetItem {
 public:
  enum { Type = UserType + 1 };

  explicit FolderItem(const String& path);

  FolderItem* AddSubfolder(const String& name);
  AbsolutePath FullPath() const;

  bool operator<(const QTreeWidgetItem& other) const override;

 private:
  HashMap<String, FolderItem*> subfolders_;
};

}  // namespace ui
}  // namespace ide
