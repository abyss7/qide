#pragma once

#include <base/alias.h>

#include <QTreeWidgetItem>

namespace ide {
namespace ui {

class FileTreeItem : public QTreeWidgetItem {
 public:
  enum { Type = UserType + 2 };

  explicit FileTreeItem(const String& name, const StringList& args,
                        bool temporary);

  String FullPath() const;
  String RelativePath() const;

  inline const std::vector<std::string>& GetArgs() const { return args_; }
  inline void SetFontBold(bool bold) {
    auto tmp_font = font(0);
    tmp_font.setBold(bold);
    setFont(0, tmp_font);
  }

  bool operator<(const QTreeWidgetItem& other) const override;

  const bool temporary;

 private:
  std::vector<std::string> args_;
};

}  // namespace ui
}  // namespace ide
