#include <ui/code_editor.h>

#include <ui/folder_tree_item.h>

#include <QDir>
#include <QMessageBox>
#include <QPalette>

#include <iostream>

namespace ide {

namespace {

using UserData = Pair<String, ui::CodeEditor::VisitorFn>;

String get_clang_string(CXString str) {
  const char* tmp = clang_getCString(str);
  if (tmp == NULL) {
    return "";
  } else {
    String translated(tmp);
    clang_disposeString(str);
    return translated;
  }
}

CXChildVisitResult visitor_helper(CXCursor cursor, CXCursor,
                                  CXClientData client_data) {
  auto& user_data = *reinterpret_cast<UserData*>(client_data);

  CXFile file;
  ui32 line, column, offset;

  // Get the file path by |cursor|.
  clang_getFileLocation(clang_getCursorLocation(cursor), &file, nullptr,
                        nullptr, nullptr);
  auto file_path =
      QFileInfo(get_clang_string(clang_getFileName(file))).absoluteFilePath();

  // Only interested in highlighting tokens in selected file.
  if (file_path != user_data.first) {
    return CXChildVisit_Continue;
  }

  // FIXME: remove this debug output.
  {
    auto range = clang_getCursorExtent(cursor);
    auto loc1 = clang_getRangeStart(range);
    auto loc2 = clang_getRangeEnd(range);

    auto name = get_clang_string(clang_getCursorDisplayName(cursor));
    auto kind = get_clang_string(
        clang_getCursorKindSpelling(clang_getCursorKind(cursor)));
    auto spelling = get_clang_string(clang_getCursorSpelling(cursor));

    clang_getSpellingLocation(loc1, &file, &line, &column, &offset);
    std::cout << line << ":" << column << "+";

    clang_getSpellingLocation(loc2, &file, &line, &column, &offset);
    std::cout << line << ":" << column << " " << kind.toStdString()
              << std::endl;

    std::cout << "\t" << name.toStdString() << ", " << spelling.toStdString()
              << std::endl;
  }

  auto unit = clang_Cursor_getTranslationUnit(cursor);
  auto cursor_range = clang_getCursorExtent(cursor);

  // Colorify only comments as tokens.
  CXToken* tokens;
  ui32 tokens_size;
  clang_tokenize(unit, cursor_range, &tokens, &tokens_size);

  if (tokens_size > 0) {
    for (auto i = 0u; i < tokens_size; i++) {
      auto token_kind = clang_getTokenKind(tokens[i]);

      if (token_kind != CXToken_Comment) {
        continue;
      }

      auto token = get_clang_string(clang_getTokenSpelling(unit, tokens[i]));
      auto token_location = clang_getTokenLocation(unit, tokens[i]);
      clang_getFileLocation(token_location, nullptr, &line, &column, nullptr);
      user_data.second(line, column, token.size(), index::ColorScheme::COMMENT);
    }
  }

  return CXChildVisit_Recurse;
}

}  // namespace

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
}

String CodeEditor::CurrentFilePath() const {
  if (item_) {
    return item_->FullPath();
  }
  return QString();
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

  if (unit_) {
    clang_disposeTranslationUnit(unit_);
    unit_ = nullptr;
  }
  if (index_) {
    clang_disposeIndex(index_);
    index_ = nullptr;
  }
}

void CodeEditor::OpenFile(QTreeWidgetItem* item, int) {
  if (!item) {
    CloseFile();
    return;
  }

  if (item->type() != FileTreeItem::Type) {
    return;
  }

  OpenFile(static_cast<FileTreeItem*>(item));
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

  auto old_position = cursor.position();

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
}

bool CodeEditor::OpenFile(FileTreeItem* item) {
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
  setEnabled(true);
  if (ParseFile()) {
    VisitFile([this](ui32 line, ui32 column, ui32 length,
                     index::ColorScheme::Kind kind) {
      HighlightToken(line, column, length, kind);
    });
  }
  textCursor().setPosition(0);
  HighlightCurrentLine();

  modificationChanged(false);

  return true;
}

bool CodeEditor::ParseFile() {
  auto suffix = QFileInfo(item_->text(0)).suffix();

  if (suffix == "cc" || suffix == "cxx" || suffix == "c++") {
    const auto c_args_size = 4096;
    const char* c_args[c_args_size];
    int size = 0;

    Q_ASSERT(c_args_size >= item_->GetArgs().size());
    for (ui32 i = 0, s = item_->GetArgs().size(); i < s; ++i) {
      c_args[size++] = item_->GetArgs()[i].c_str();
    }

    index_ = clang_createIndex(0, 1);
    auto error = clang_parseTranslationUnit2(
        index_, nullptr, c_args, size, nullptr, 0,
        CXTranslationUnit_DetailedPreprocessingRecord, &unit_);

    return error == CXError_Success;
  }

  // Don't know, how to parse the file.
  return false;
}

void CodeEditor::VisitFile(VisitorFn visitor) {
  CXCursor cursor = clang_getTranslationUnitCursor(unit_);
  UserData user_data(item_->FullPath(), visitor);
  clang_visitChildren(cursor, visitor_helper, &user_data);
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

}  // namespace ui
}  // namespace ide
