#pragma once

#include <base/alias.h>
#include <index/color_scheme.h>

#include <clang/Frontend/ASTUnit.h>

namespace ide {
namespace index {

class ClangParser {
 public:
  using VisitorFn =
      Fn<void(ui32 line, ui32 column, ui32 length, ColorScheme::Kind)>;

  explicit ClangParser(const std::vector<std::string>& args);

  void Visit(VisitorFn visitor);

 private:
  std::unique_ptr<clang::ASTUnit> unit_;
};

}  // namespace index
}  // namespace ide
