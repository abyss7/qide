#pragma once

#include <QWidget>

class CodeEditor;

class LineNumberArea : public QWidget {
 public:
  explicit LineNumberArea(CodeEditor* editor);

  int CalculateWidth() const;

 private:
  void paintEvent(QPaintEvent* event) override;
  QSize sizeHint() const override;

  CodeEditor* code_editor_;
};
