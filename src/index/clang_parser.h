#pragma once

#include <base/alias.h>
#include <index/color_scheme.h>

#include <vector>

namespace ide {
namespace index {

class ClangParser {
 public:
  using ColorFn =
      Fn<void(ui32 line, ui32 column, ui32 length, ColorScheme::Kind)>;

  explicit ClangParser(const std::vector<std::string>& args);

  void Colorify(ColorFn visitor, String code = "");
};

}  // namespace index
}  // namespace ide
