#include <ui/line_number_area.h>

#include <ui/code_editor.h>

#include <QPainter>
#include <QTextBlock>

namespace ide {
namespace ui {

LineNumberArea::LineNumberArea(CodeEditor* editor)
    : QWidget(editor), code_editor_(editor) {
}

int LineNumberArea::CalculateWidth() const {
  int digits = 1;
  int max = qMax(1, code_editor_->blockCount());
  while (max >= 10) {
    max /= 10;
    ++digits;
  }

  return 13 + code_editor_->fontMetrics().width(QLatin1Char('9')) * digits;
}

QSize LineNumberArea::sizeHint() const {
  return QSize(CalculateWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent* event) {
  if (!code_editor_->isEnabled())
    return;

  auto line_number_color =
      code_editor_->scheme_[index::ColorScheme::LINE_NUMBER];
  QPainter painter(this);
  painter.fillRect(event->rect(), line_number_color.bg);

  QTextBlock block = code_editor_->firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = code_editor_->blockBoundingGeometry(block)
                .translated(code_editor_->contentOffset())
                .top();
  int bottom = top + code_editor_->blockBoundingRect(block).height();
  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
      String number = QString::number(blockNumber + 1);
      painter.setPen(line_number_color.fg);
      painter.drawText(0, top, width() - 7,
                       code_editor_->fontMetrics().height(), Qt::AlignRight,
                       number);
    }

    block = block.next();
    top = bottom;
    bottom = top + code_editor_->blockBoundingRect(block).height();
    ++blockNumber;
  }
}

}  // namespace ui
}  // namespace ide
