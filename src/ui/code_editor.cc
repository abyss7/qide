#include <ui/code_editor.h>

#include <ui/folder_item.h>

#include <QDir>
#include <QMessageBox>
#include <QPalette>

#include <iostream>

namespace ide {
namespace ui {

CodeEditor::CodeEditor(QWidget* parent)
    : QPlainTextEdit(parent),
      line_number_area_(new LineNumberArea(this)),
      scheme_(Qt::gray, Qt::black) {
  UpdateLineNumberAreaWidth(0);

  auto new_palette = palette();
  new_palette.setColor(QPalette::Active, QPalette::Base, Qt::black);
  new_palette.setColor(QPalette::Inactive, QPalette::Base, Qt::black);
  setPalette(new_palette);

  QTextCharFormat format;
  format.setForeground(scheme_[index::ColorScheme::DEFAULT].fg);
  format.setBackground(scheme_[index::ColorScheme::DEFAULT].bg);
  setCurrentCharFormat(format);

  // Fill-up the default color scheme.
  scheme_[index::ColorScheme::LINE_NUMBER] = {Qt::gray, QColor("#2B1B17")};
  scheme_[index::ColorScheme::HIGHLIGHT_LINE] = {Qt::gray, QColor("#2B1B17")};
  scheme_[index::ColorScheme::COMMENT] = {Qt::cyan, Qt::black};
  scheme_[index::ColorScheme::KEYWORD] = {Qt::yellow, Qt::black};
  scheme_[index::ColorScheme::NAMESPACE] = {Qt::green, Qt::black};
  scheme_[index::ColorScheme::NUMBER_LITERAL] = {Qt::magenta, Qt::black};
  scheme_[index::ColorScheme::STRING_LITERAL] = {Qt::magenta, Qt::black};
}

AbsolutePath CodeEditor::CurrentFilePath() const {
  if (item_) {
    return item_->FullPath();
  }

  // FIXME: not reached.
  return AbsolutePath(String());
}

void CodeEditor::CloseFile() {
  SaveFile();

  if (item_) {
    clear();
    item_->SetFontBold(false);
    item_ = nullptr;
  }

  setEnabled(false);
  setExtraSelections({});
}

void CodeEditor::OpenFile(QTreeWidgetItem* item, int) {
  if (!item) {
    CloseFile();
    return;
  }

  if (item->type() != FileItem::Type) {
    return;
  }

  OpenFile(static_cast<FileItem*>(item));
}

void CodeEditor::SaveFile() {
  if (item_) {
    QFile file(CurrentFilePath() + ".save");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text |
                  QIODevice::Truncate)) {
      file.write(toPlainText().toUtf8());
      file.flush();

      QDir dir;
      if (!dir.remove(CurrentFilePath()) ||
          !dir.rename(file.fileName(), CurrentFilePath())) {
        QMessageBox::warning(this, "Error", "Can't rename " + file.fileName() +
                                                " to " + CurrentFilePath());
      }
    } else {
      QMessageBox::warning(this, "Error", "Can't save opened file!");
    }

    modificationChanged(false);
  }
}

void CodeEditor::resizeEvent(QResizeEvent* event) {
  QPlainTextEdit::resizeEvent(event);

  QRect contents_rect = contentsRect();
  contents_rect.setWidth(line_number_area_->CalculateWidth());
  line_number_area_->setGeometry(contents_rect);
}

void CodeEditor::HighlightToken(ui32 line, ui32 column, ui32 length,
                                index::ColorScheme::Kind kind) {
  QTextCursor cursor(document());

  setUndoRedoEnabled(false);
  auto old_position = textCursor().position();

  cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
  cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,
                      line - 1);
  cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column - 1);
  cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, length);
  setTextCursor(cursor);

  QTextCharFormat format;
  format.setForeground(scheme_[kind].fg);
  format.setBackground(scheme_[kind].bg);
  setCurrentCharFormat(format);

  // Restore previous state.
  cursor.setPosition(old_position);
  setTextCursor(cursor);

  format.setForeground(scheme_[index::ColorScheme::DEFAULT].fg);
  format.setBackground(scheme_[index::ColorScheme::DEFAULT].bg);
  setCurrentCharFormat(format);

  setUndoRedoEnabled(true);
}

bool CodeEditor::OpenFile(FileItem* item) {
  CloseFile();

  item_ = item;
  item_->SetFontBold(true);

  QFile file(CurrentFilePath());
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    // TODO: disable item in the Project Tree.
    QMessageBox::warning(this, "Error", "Can't open selected file!");
    return false;
  }

  setPlainText(file.readAll());

  Colorify();

  setEnabled(true);
  textCursor().setPosition(0);
  HighlightCurrentLine();

  document()->setModified(false);

  return true;
}

void CodeEditor::HighlightCurrentLine() {
  List<QTextEdit::ExtraSelection> extra_selections;

  if (!isReadOnly() && isEnabled()) {
    QTextEdit::ExtraSelection selection;

    selection.format.setBackground(
        scheme_[index::ColorScheme::HIGHLIGHT_LINE].bg);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extra_selections.append(selection);
  }

  setExtraSelections(extra_selections);
}

void CodeEditor::UpdateLineNumberArea(const QRect& rect, int dy) {
  if (dy) {
    line_number_area_->scroll(0, dy);
  } else {
    line_number_area_->update(0, rect.y(), line_number_area_->width(),
                              rect.height());
  }

  if (rect.contains(viewport()->rect())) {
    UpdateLineNumberAreaWidth(0);
  }
}

void CodeEditor::UpdateLineNumberAreaWidth(int) {
  setViewportMargins(line_number_area_->CalculateWidth(), 0, 0, 0);
}

void CodeEditor::Colorify() {
  // FIXME: do nothing for now.
}

}  // namespace ui
}  // namespace ide
