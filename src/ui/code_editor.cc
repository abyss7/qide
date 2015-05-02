#include <ui/code_editor.h>

#include <ui/file_tree_item.h>
#include <ui/folder_tree_item.h>

#include <QDir>
#include <QMessageBox>

CodeEditor::CodeEditor(QWidget* parent)
    : QPlainTextEdit(parent), line_number_area_(new LineNumberArea(this)) {
  UpdateLineNumberAreaWidth(0);
}

QString CodeEditor::CurrentFilePath() const {
  if (current_item_) {
    return current_item_->FullPath();
  }
  return QString();
}

bool CodeEditor::OpenFile(FileTreeItem* item) {
  CloseFile();

  current_item_ = item;
  current_item_->SetFontBold(true);

  QFile file(CurrentFilePath());
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    // TODO: disable item in the Project Tree.
    QMessageBox::warning(this, "Error", "Can't open selected file!");
    return false;
  }

  setPlainText(file.readAll());
  setEnabled(true);
  HighlightCurrentLine();

  return true;
}

void CodeEditor::CloseFile() {
  SaveFile();

  if (current_item_) {
    clear();
    current_item_->SetFontBold(false);
    current_item_ = nullptr;
  }

  setEnabled(false);
}

void CodeEditor::SaveFile() {
  if (current_item_) {
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
  }
}

void CodeEditor::resizeEvent(QResizeEvent* event) {
  QPlainTextEdit::resizeEvent(event);

  QRect contents_rect = contentsRect();
  contents_rect.setWidth(line_number_area_->CalculateWidth());
  line_number_area_->setGeometry(contents_rect);
}

void CodeEditor::HighlightCurrentLine() {
  QList<QTextEdit::ExtraSelection> extra_selections;

  if (!isReadOnly() && isEnabled()) {
    QTextEdit::ExtraSelection selection;

    QColor line_color = QColor(Qt::yellow).lighter(160);

    selection.format.setBackground(line_color);
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
