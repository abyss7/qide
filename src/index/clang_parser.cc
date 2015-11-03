#include <index/clang_parser.h>

#include <base/alias.h>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Lex/Lexer.h>
#include <clang/Tooling/Tooling.h>

namespace ide {
namespace index {

ClangParser::ClangParser(const std::vector<std::string>& args) : args_(args) {}

void ClangParser::Colorify(ColorFn visitor) {
  auto helper = [visitor](const clang::CompilerInstance& instance,
                          const clang::Token& token) {
    const auto& source_manager = instance.getSourceManager();
    auto location = token.getLocation();

    if (!source_manager.isInMainFile(location) ||
        source_manager.isMacroBodyExpansion(location)) {
      return;
    }

    auto line = source_manager.getSpellingLineNumber(location);
    auto column = source_manager.getSpellingColumnNumber(location);

    switch (token.getKind()) {
      // Comments.
      case clang::tok::comment:
        visitor(line, column, token.getLength(), index::ColorScheme::COMMENT);
        break;

      // Keywords.
      case clang::tok::kw_break:
      case clang::tok::kw_case:
      case clang::tok::kw_class:
      case clang::tok::kw_default:
      case clang::tok::kw_else:
      case clang::tok::kw_false:
      case clang::tok::kw_for:
      case clang::tok::kw_if:
      case clang::tok::kw_namespace:
      case clang::tok::kw_new:
      case clang::tok::kw_return:
      case clang::tok::kw_struct:
      case clang::tok::kw_switch:
      case clang::tok::kw_this:
      case clang::tok::kw_true:
      case clang::tok::kw_using:
      case clang::tok::kw_while:
        visitor(line, column, token.getLength(), index::ColorScheme::KEYWORD);
        break;

      // Types.
      case clang::tok::kw_auto:
      case clang::tok::kw_bool:
      case clang::tok::kw_char:
      case clang::tok::kw_const:
      case clang::tok::kw_int:
      case clang::tok::kw_static:
      case clang::tok::kw_void:
        visitor(line, column, token.getLength(), index::ColorScheme::KEYWORD);
        break;

      // Literals.
      case clang::tok::numeric_constant:
        visitor(line, column, token.getLength(),
                index::ColorScheme::NUMBER_LITERAL);
        break;
      case clang::tok::string_literal:
        visitor(line, column, token.getLength(),
                index::ColorScheme::STRING_LITERAL);
        break;

      // Other.
      default:
        visitor(line, column, token.getLength(), index::ColorScheme::DEFAULT);
    }
  };

  clang::tooling::runToolOnCodeWithArgs(new clang::HandleTokensAction(helper),
                                        "", args_, "");
}

}  // namespace index
}  // namespace ide
