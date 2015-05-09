#include <project/clang_index.h>

namespace ide {

namespace {

using UserData = Pair<String, ClangIndex::VisitorFn>;

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

ClangIndex::~ClangIndex() {
  for (const auto& unit : units_) {
    clang_disposeTranslationUnit(unit);
  }
  clang_disposeIndex(index_);
}

void ClangIndex::Parse(const String& path, const StringList& args) {
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

void ClangIndex::Visit(const String& path, VisitorFn visitor) {
  if (!units_.contains(path)) {
    return;
  }

  CXCursor cursor = clang_getTranslationUnitCursor(units_[path]);
  UserData user_data(path, visitor);
  clang_visitChildren(cursor, visitor_helper, &user_data);
}

}  // namespace ide
