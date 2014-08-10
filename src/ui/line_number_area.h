#pragma once

#include <QWidget>

class CodeEditor;

class LineNumberArea : public QWidget {
 public:
  explicit LineNumberArea(CodeEditor *editor);

  int CalculateWidth() const;

 private:
  virtual void paintEvent(QPaintEvent *event) override;
  virtual QSize sizeHint() const override;

  CodeEditor *code_editor_;
};
