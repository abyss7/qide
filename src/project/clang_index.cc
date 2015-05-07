#include <project/clang_index.h>

namespace ide {

namespace {

using UserData = QPair<QString, ClangIndex::VisitorFn>;

std::string get_clang_string(CXString str) {
  const char* tmp = clang_getCString(str);
  if (tmp == NULL) {
    return "";
  } else {
    std::string translated = std::string(tmp);
    clang_disposeString(str);
    return translated;
  }
}

CXChildVisitResult visitor_helper(CXCursor cursor, CXCursor parent,
                                  CXClientData client_data) {
  auto& user_data = *reinterpret_cast<UserData*>(client_data);

  CXFile file;
  unsigned int line;
  unsigned int column;
  unsigned int offset;

  CXSourceLocation loc = clang_getCursorLocation(cursor);
  clang_getFileLocation(loc, &file, &line, &column, &offset);

  // Only interested in highlighting tokens in selected file.
  if (QString::fromStdString(get_clang_string(clang_getFileName(file))) !=
      user_data.first) {
    return CXChildVisit_Continue;
  }

  CXTranslationUnit tu = clang_Cursor_getTranslationUnit(cursor);
  CXSourceRange range = clang_getCursorExtent(cursor);

  CXToken* tokens;
  unsigned int tokens_size;
  clang_tokenize(tu, range, &tokens, &tokens_size);

  if (tokens_size > 0) {
    for (unsigned int i = 0; i < tokens_size - 1; i++) {
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

ClangIndex::~ClangIndex() {
  for (const auto& unit : units_) {
    clang_disposeTranslationUnit(unit);
  }
  clang_disposeIndex(index_);
}

void ClangIndex::Parse(const QString& path, const QStringList& args) {
  if (units_.contains(path)) {
    return;
  }

  const char* c_args[1024];
  int size = 0;
  for (const auto& arg : args) {
    c_args[size++] = arg.toStdString().c_str();
  }
  units_.insert(path, clang_parseTranslationUnit(
                          index_, path.toStdString().c_str(), c_args, size,
                          nullptr, 0, CXTranslationUnit_None));
}

void ClangIndex::Visit(const QString& path, VisitorFn visitor) {
  if (!units_.contains(path)) {
    return;
  }

  CXCursor cursor = clang_getTranslationUnitCursor(units_[path]);
  UserData user_data(path, visitor);
  clang_visitChildren(cursor, visitor_helper, &user_data);
}

}  // namespace ide
