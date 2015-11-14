#pragma once

#include <project/base_project.h>

#include <QTreeWidgetItem>

namespace ide {
namespace ui {

class FileItem : public QTreeWidgetItem {
 public:
  enum { Type = UserType + 2 };

  explicit FileItem(const String& name, Project::Iterator it,
                        bool temporary);

  AbsolutePath FullPath() const;
  RelativePath RelativePath() const;

  inline StringList args() const { return it_.args(); }
  inline void SetFontBold(bool bold) {
    auto tmp_font = font(0);
    tmp_font.setBold(bold);
    setFont(0, tmp_font);
  }

  bool operator<(const QTreeWidgetItem& other) const override;

  const bool temporary;

 private:
  Project::Iterator it_;
};

}  // namespace ui
}  // namespace ide
