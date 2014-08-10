#pragma once

#include <ui/line_number_area.h>

#include <QPlainTextEdit>

#include <memory>

class FileTreeItem;
class QTreeWidgetItem;

class CodeEditor : public QPlainTextEdit {
  Q_OBJECT

 public:
  explicit CodeEditor(QWidget* parent = nullptr);

  QString CurrentFilePath() const;
  bool OpenFile(FileTreeItem* item);
  void CloseFile();

 public slots:
  void SaveFile();

 protected:
  virtual void resizeEvent(QResizeEvent* event) override;

 private:
  friend class LineNumberArea;

  FileTreeItem* current_item_ = nullptr;
  std::unique_ptr<LineNumberArea> line_number_area_;

 private slots:
  void HighlightCurrentLine();
  void UpdateLineNumberArea(const QRect& rect, int dy);
  void UpdateLineNumberAreaWidth(int);
};
