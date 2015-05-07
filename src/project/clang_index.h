#pragma once

#include <QHash>
#include <QStringList>

#include <clang-c/Index.h>

namespace ide {

class ClangIndex {
 public:
  using VisitorFn = std::function<void(unsigned line, unsigned column,
                                       unsigned length, CXTokenKind)>;

  ~ClangIndex();

  void Parse(const QString& path, const QStringList& args);
  void Visit(const QString& path, VisitorFn visitor);

 private:
  CXIndex index_ = clang_createIndex(0, 0);
  QHash<QString, CXTranslationUnit> units_;
};

}  // namespace ide
