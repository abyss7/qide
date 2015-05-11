#pragma once

#include <base/alias.h>

#include <ui/file_tree_item.h>
#include <ui/line_number_area.h>

#include <QPlainTextEdit>

namespace ide {
namespace ui {

class CodeEditor : public QPlainTextEdit {
  Q_OBJECT

 public:
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

  FileTreeItem* current_item_ = nullptr;
  UniquePtr<LineNumberArea> line_number_area_;

 private slots:
  void HighlightCurrentLine();
  void UpdateLineNumberArea(const QRect& rect, int dy);
  void UpdateLineNumberAreaWidth(int);
};

}  // namespace ui
}  // namespace ide
