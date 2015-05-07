#pragma once

#include <QTreeWidgetItem>

class FileTreeItem : public QTreeWidgetItem {
 public:
  enum { Type = UserType + 2 };

  explicit FileTreeItem(const QString& name, bool temporary);

  QString FullPath() const;

  inline void SetFontBold(bool bold) {
    auto tmp_font = font(0);
    tmp_font.setBold(bold);
    setFont(0, tmp_font);
  }

  bool operator<(const QTreeWidgetItem& other) const override;
};
