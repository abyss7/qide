#pragma once

#include <QHash>
#include <QTreeWidgetItem>

class FolderTreeItem : public QTreeWidgetItem {
 public:
  enum { Type = UserType + 1 };

  explicit FolderTreeItem(const QString& path);

  FolderTreeItem* AddSubfolder(const QString& name);
  QString FullPath() const;

  bool operator<(const QTreeWidgetItem& other) const override;

 private:
  QHash<QString, FolderTreeItem*> subfolders_;
};
