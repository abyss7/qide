#pragma once

#include <base/path.h>
#include <index/color_scheme.h>
#include <ui/file_item.h>
#include <ui/line_number_area.h>

#include <QPlainTextEdit>
#include <QShortcut>

namespace ide {
namespace ui {

class CodeEditor : public QPlainTextEdit {
  Q_OBJECT

 public:
  explicit CodeEditor(QWidget* parent = nullptr);

  AbsolutePath CurrentFilePath() const;
  void CloseFile();

 public slots:
  void OpenFile(QTreeWidgetItem* item, int column);
  void SaveFile();

 protected:
  void resizeEvent(QResizeEvent* event) override;

 private:
  friend class LineNumberArea;

  void HighlightToken(ui32 line, ui32 column, ui32 length,
                      index::ColorScheme::Kind kind);
  bool OpenFile(FileItem* item);

  FileItem* item_ = nullptr;
  UniquePtr<LineNumberArea> line_number_area_;
  QShortcut shortcut_;
  index::ColorScheme scheme_;

 private slots:
  void HighlightCurrentLine();
  void UpdateLineNumberArea(const QRect& rect, int dy);
  void UpdateLineNumberAreaWidth(int);
  void Colorify();
  void CodeComplete();
};

}  // namespace ui
}  // namespace ide
