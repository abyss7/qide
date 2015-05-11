#include <ui/file_tree_item.h>

#include <QDir>

namespace ide {

namespace {

using UserData = Pair<String, ui::FileTreeItem::VisitorFn>;

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

CXChildVisitResult visitor_helper(CXCursor cursor, CXCursor parent,
                                  CXClientData client_data) {
  auto& user_data = *reinterpret_cast<UserData*>(client_data);

  CXFile file;
  ui32 line, column, offset;

  CXSourceLocation location = clang_getCursorLocation(cursor);
  clang_getFileLocation(location, &file, &line, &column, &offset);

  // Only interested in highlighting tokens in selected file.
  if (get_clang_string(clang_getFileName(file)) != user_data.first) {
    return CXChildVisit_Continue;
  }

  CXTranslationUnit tu = clang_Cursor_getTranslationUnit(cursor);
  CXSourceRange range = clang_getCursorExtent(cursor);

  CXToken* tokens;
  ui32 tokens_size;
  clang_tokenize(tu, range, &tokens, &tokens_size);

  if (tokens_size > 0) {
    for (auto i = 0u; i < tokens_size - 1; i++) {
      auto token = get_clang_string(clang_getTokenSpelling(tu, tokens[i]));
      CXSourceLocation tl = clang_getTokenLocation(tu, tokens[i]);

      clang_getFileLocation(tl, &file, &line, &column, &offset);

      user_data.second(line, column, token.size(),
                       clang_getTokenKind(tokens[i]));
    }
  }

  return CXChildVisit_Continue;
}

}  // namespace

namespace ui {

FileTreeItem::FileTreeItem(const String& name, const StringList& args,
                           bool temporary)
    : QTreeWidgetItem(Type), temporary(temporary) {
  setText(0, name);
  setToolTip(0, name);
  setIcon(0, QIcon::fromTheme("text-x-generic"));

  auto tmp_font = font(0);
  tmp_font.setUnderline(!temporary);
  setFont(0, tmp_font);

  auto suffix = QFileInfo(name).suffix();

  if (suffix == "cc" || suffix == "cxx" || suffix == "c++") {
    const auto c_args_size = 4096;
    const char* c_args[c_args_size];
    int size = 0;

    Q_ASSERT(c_args_size >= args.size());
    for (const auto& arg : args) {
      args_.push_back(arg.toStdString());
      c_args[size++] = args_.back().c_str();
    }

    unit_ = clang_parseTranslationUnit(index_, FullPath().toStdString().c_str(),
                                       c_args, size, nullptr, 0,
                                       CXTranslationUnit_None);
  }
}

FileTreeItem::~FileTreeItem() {
  clang_disposeTranslationUnit(unit_);
  clang_disposeIndex(index_);
}

String FileTreeItem::FullPath() const {
  String full_path = text(0);
  const QTreeWidgetItem* item = this;
  while (item->parent()) {
    item = item->parent();
    full_path = item->text(0) + QDir::separator() + full_path;
  }

  return full_path;
}

String FileTreeItem::RelativePath() const {
  QString full_path = text(0);
  const QTreeWidgetItem* item = this;
  while (item->parent()->parent()) {
    item = item->parent();
    full_path = item->text(0) + QDir::separator() + full_path;
  }

  return full_path;
}

void FileTreeItem::Visit(VisitorFn visitor) {
  CXCursor cursor = clang_getTranslationUnitCursor(unit_);
  UserData user_data(FullPath(), visitor);
  clang_visitChildren(cursor, visitor_helper, &user_data);
}

bool FileTreeItem::operator<(const QTreeWidgetItem& other) const {
  if (treeWidget()->sortColumn() == 0 && other.type() != Type) {
    return false;
  }

  // FIXME: not reached.
  return QTreeWidgetItem::operator<(other);
}

}  // namespace ui
}  // namespace ide
