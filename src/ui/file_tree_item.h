#pragma once

#include <base/alias.h>

#include <QTreeWidgetItem>

#include <clang-c/Index.h>

namespace ide {
namespace ui {

class FileTreeItem : public QTreeWidgetItem {
 public:
  enum { Type = UserType + 2 };

  using VisitorFn = Fn<void(ui32 line, ui32 column, ui32 length, CXTokenKind)>;

  explicit FileTreeItem(const String& name, const StringList& args,
                        bool temporary);
  ~FileTreeItem();

  String FullPath() const;
  String RelativePath() const;

  bool Parse(CXIndex index);
  void Visit(VisitorFn visitor);

  inline void SetFontBold(bool bold) {
    auto tmp_font = font(0);
    tmp_font.setBold(bold);
    setFont(0, tmp_font);
  }

  bool operator<(const QTreeWidgetItem& other) const override;

  const bool temporary;

 private:
  List<std::string> args_;
  CXTranslationUnit unit_;
};

}  // namespace ui
}  // namespace ide
