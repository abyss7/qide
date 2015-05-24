#pragma once

#include <base/alias.h>

#include <ui/file_tree_item.h>
#include <ui/line_number_area.h>

#include <QPlainTextEdit>

#include <clang-c/Index.h>

namespace ide {
namespace ui {

class CodeEditor : public QPlainTextEdit {
  Q_OBJECT

 public:
  using VisitorFn = Fn<void(ui32 line, ui32 column, ui32 length, CXTokenKind)>;

  explicit CodeEditor(QWidget* parent = nullptr);

  String CurrentFilePath() const;
  void CloseFile();

 public slots:
  void OpenFile(QTreeWidgetItem* item, int column);
  void SaveFile();

 protected:
  void resizeEvent(QResizeEvent* event) override;

 private:
  friend class LineNumberArea;

  void HighlightToken(ui32 line, ui32 column, ui32 length, CXTokenKind kind);
  bool OpenFile(FileTreeItem* item);

  // FIXME: move those methods inside the |OpenFile()| method.
  bool ParseFile();
  void VisitFile(VisitorFn visitor);

  FileTreeItem* item_ = nullptr;
  CXIndex index_ = nullptr;
  CXTranslationUnit unit_ = nullptr;
  UniquePtr<LineNumberArea> line_number_area_;

 private slots:
  void HighlightCurrentLine();
  void UpdateLineNumberArea(const QRect& rect, int dy);
  void UpdateLineNumberAreaWidth(int);
};

}  // namespace ui
}  // namespace ide
