#pragma once

#include <base/alias.h>

#include <QHash>

#include <clang-c/Index.h>

namespace ide {

class ClangIndex {
 public:
  using VisitorFn = Fn<void(ui32 line, ui32 column, ui32 length, CXTokenKind)>;

  ~ClangIndex();

  void Parse(const String& path, const StringList& args);
  void Visit(const String& path, VisitorFn visitor);

 private:
  CXIndex index_ = clang_createIndex(0, 0);
  HashMap<String, CXTranslationUnit> units_;
};

}  // namespace ide
