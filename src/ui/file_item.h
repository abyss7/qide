#pragma once

#include <index/clang_parser.h>
#include <project/base_project.h>

#include <QTreeWidgetItem>

namespace ide {
namespace ui {

class FileItem : public QTreeWidgetItem {
 public:
  enum { Type = UserType + 2 };

  explicit FileItem(const String& name, Project::Iterator it,
                    index::ClangParser* parser);

  AbsolutePath FullPath() const;
  RelativePath RelativePath() const;

  void CodeComplete(ui32 line, ui32 column);

  inline void SetFontBold(bool bold) {
    auto tmp_font = font(0);
    tmp_font.setBold(bold);
    setFont(0, tmp_font);
  }

  bool operator<(const QTreeWidgetItem& other) const override;

  const bool temporary;

 private:
  Project::Iterator it_;
  index::ClangParser* parser_ = nullptr;
};

}  // namespace ui
}  // namespace ide
