#include <ui/file_tree_item.h>

#include <QDir>

#include <iostream>

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
  if (QFileInfo(get_clang_string(clang_getFileName(file))).absoluteFilePath() !=
      user_data.first) {
    return CXChildVisit_Continue;
  }

  CXTranslationUnit tu = clang_Cursor_getTranslationUnit(cursor);
  CXSourceRange range = clang_getCursorExtent(cursor);

  CXToken* tokens;
  ui32 tokens_size;
  clang_tokenize(tu, range, &tokens, &tokens_size);

  if (tokens_size > 0) {
    for (auto i = 0u; i < tokens_size; i++) {
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

  for (ui32 i = 0, s = args.size(); i < s; ++i) {
    if (i == 0) {
      // Skip the first argument - usually it's a "clang++".
      continue;
    }
    args_.push_back(args[i].toStdString());
  }
}

FileTreeItem::~FileTreeItem() {
  if (unit_) {
    clang_disposeTranslationUnit(unit_);
  }
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

bool FileTreeItem::Parse(CXIndex index) {
  auto suffix = QFileInfo(text(0)).suffix();

  if (suffix == "cc" || suffix == "cxx" || suffix == "c++") {
    const auto c_args_size = 4096;
    const char* c_args[c_args_size];
    int size = 0;

    Q_ASSERT(c_args_size >= args_.size());
    for (ui32 i = 0, s = args_.size(); i < s; ++i) {
      c_args[size++] = args_[i].c_str();
    }

    auto error =
        clang_parseTranslationUnit2(index, nullptr, c_args, size, nullptr, 0,
                                    CXTranslationUnit_None, &unit_);
    if (error != CXError_Success) {
      std::cout << "Error while parsing " << RelativePath().toStdString()
                << std::endl;
    } else {
      std::cout << "Successfully parsed " << RelativePath().toStdString()
                << std::endl;
    }

    return error == CXError_Success;
  }

  // Don't know, how to parse the file.
  return false;
}

void FileTreeItem::Visit(VisitorFn visitor) {
  CXCursor cursor = clang_getTranslationUnitCursor(unit_);
  UserData user_data(FullPath(), visitor);
  if (clang_visitChildren(cursor, visitor_helper, &user_data)) {
    std::cerr << "Error while visiting " << RelativePath().toStdString()
              << std::endl;
  }
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
