#include <index/clang_parser.h>

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Lex/Lexer.h>
#include <clang/Parse/ParseAST.h>
#include <clang/Tooling/Tooling.h>

#include <iostream>

namespace ide {

namespace {

class Visitor : public clang::RecursiveASTVisitor<Visitor> {
 public:
  Visitor(index::ClangParser::VisitorFn visitor,
          const clang::ASTContext& context)
      : visitor_(visitor), context_(context) {}

  // Declaration visitors. Keep in alphabetical order.

  bool VisitCXXRecordDecl(clang::CXXRecordDecl* decl) {
    const auto& source_manager = context_.getSourceManager();
    const auto& lang_opts = context_.getLangOpts();
    auto location = decl->getInnerLocStart();
    clang::Token token;

    if (!source_manager.isInMainFile(location)) {
      return true;
    }

    if (!source_manager.isMacroBodyExpansion(location)) {
      clang::Lexer::getRawToken(location, token, source_manager, lang_opts);

      auto line = source_manager.getSpellingLineNumber(location);
      auto column = source_manager.getSpellingColumnNumber(location);
      visitor_(line, column, token.getLength(), index::ColorScheme::KEYWORD);
    }

    return true;
  }

  bool VisitNamespaceDecl(clang::NamespaceDecl* decl) {
    const auto& source_manager = context_.getSourceManager();
    const auto& lang_opts = context_.getLangOpts();
    auto location = decl->getLocStart();
    clang::Token token;

    if (!source_manager.isInMainFile(location)) {
      return true;
    }

    if (!source_manager.isMacroBodyExpansion(location)) {
      clang::Lexer::getRawToken(location, token, source_manager, lang_opts);

      auto line = source_manager.getSpellingLineNumber(location);
      auto column = source_manager.getSpellingColumnNumber(location);
      visitor_(line, column, token.getLength(), index::ColorScheme::KEYWORD);
    }

    location = decl->getLocation();
    if (!source_manager.isMacroBodyExpansion(location) &&
        !decl->isAnonymousNamespace()) {
      clang::Lexer::getRawToken(location, token, source_manager, lang_opts);

      auto line = source_manager.getSpellingLineNumber(location);
      auto column = source_manager.getSpellingColumnNumber(location);
      visitor_(line, column, token.getLength(), index::ColorScheme::NAMESPACE);
    }

    return true;
  }

  // Statement visitors. Keep in alphabetical order.

  bool VisitCaseStmt(clang::CaseStmt* statement) {
    const auto& source_manager = context_.getSourceManager();
    auto location = statement->getKeywordLoc();

    if (!source_manager.isInMainFile(location)) {
      return true;
    }

    if (!source_manager.isMacroBodyExpansion(location)) {
      clang::Token token;
      const auto& lang_opts = context_.getLangOpts();
      clang::Lexer::getRawToken(location, token, source_manager, lang_opts);

      auto line = source_manager.getSpellingLineNumber(location);
      auto column = source_manager.getSpellingColumnNumber(location);
      visitor_(line, column, token.getLength(), index::ColorScheme::KEYWORD);
    }

    return true;
  }

  bool VisitCXXForRangeStmt(clang::CXXForRangeStmt* statement) {
    const auto& source_manager = context_.getSourceManager();
    auto location = statement->getForLoc();

    if (!source_manager.isInMainFile(location)) {
      return true;
    }

    if (!source_manager.isMacroBodyExpansion(location)) {
      clang::Token token;
      const auto& lang_opts = context_.getLangOpts();
      clang::Lexer::getRawToken(location, token, source_manager, lang_opts);

      auto line = source_manager.getSpellingLineNumber(location);
      auto column = source_manager.getSpellingColumnNumber(location);
      visitor_(line, column, token.getLength(), index::ColorScheme::KEYWORD);
    }

    return true;
  }

  bool VisitDefaultStmt(clang::DefaultStmt* statement) {
    const auto& source_manager = context_.getSourceManager();
    auto location = statement->getKeywordLoc();

    if (!source_manager.isInMainFile(location)) {
      return true;
    }

    if (!source_manager.isMacroBodyExpansion(location)) {
      clang::Token token;
      const auto& lang_opts = context_.getLangOpts();
      clang::Lexer::getRawToken(location, token, source_manager, lang_opts);

      auto line = source_manager.getSpellingLineNumber(location);
      auto column = source_manager.getSpellingColumnNumber(location);
      visitor_(line, column, token.getLength(), index::ColorScheme::KEYWORD);
    }

    return true;
  }

  bool VisitForStmt(clang::ForStmt* statement) {
    const auto& source_manager = context_.getSourceManager();
    auto location = statement->getForLoc();

    if (!source_manager.isInMainFile(location)) {
      return true;
    }

    if (!source_manager.isMacroBodyExpansion(location)) {
      clang::Token token;
      const auto& lang_opts = context_.getLangOpts();
      clang::Lexer::getRawToken(location, token, source_manager, lang_opts);

      auto line = source_manager.getSpellingLineNumber(location);
      auto column = source_manager.getSpellingColumnNumber(location);
      visitor_(line, column, token.getLength(), index::ColorScheme::KEYWORD);
    }

    return true;
  }

  bool VisitIfStmt(clang::IfStmt* statement) {
    const auto& source_manager = context_.getSourceManager();
    auto if_location = statement->getIfLoc();
    auto else_location = statement->getElseLoc();

    if (!source_manager.isInMainFile(if_location)) {
      return true;
    }

    if (!source_manager.isMacroBodyExpansion(if_location)) {
      clang::Token token;
      const auto& lang_opts = context_.getLangOpts();
      clang::Lexer::getRawToken(if_location, token, source_manager, lang_opts);

      auto line = source_manager.getSpellingLineNumber(if_location);
      auto column = source_manager.getSpellingColumnNumber(if_location);
      visitor_(line, column, token.getLength(), index::ColorScheme::KEYWORD);
    }

    if (else_location.isValid() &&
        !source_manager.isMacroBodyExpansion(else_location)) {
      clang::Token token;
      const auto& lang_opts = context_.getLangOpts();
      clang::Lexer::getRawToken(else_location, token, source_manager,
                                lang_opts);

      auto line = source_manager.getSpellingLineNumber(else_location);
      auto column = source_manager.getSpellingColumnNumber(else_location);
      visitor_(line, column, token.getLength(), index::ColorScheme::KEYWORD);
    }

    return true;
  }

  bool VisitReturnStmt(clang::ReturnStmt* statement) {
    const auto& source_manager = context_.getSourceManager();
    auto location = statement->getReturnLoc();

    if (!source_manager.isInMainFile(location)) {
      return true;
    }

    if (!source_manager.isMacroBodyExpansion(location)) {
      clang::Token token;
      const auto& lang_opts = context_.getLangOpts();
      clang::Lexer::getRawToken(location, token, source_manager, lang_opts);

      auto line = source_manager.getSpellingLineNumber(location);
      auto column = source_manager.getSpellingColumnNumber(location);
      visitor_(line, column, token.getLength(), index::ColorScheme::KEYWORD);
    }

    return true;
  }

  bool VisitSwitchStmt(clang::SwitchStmt* statement) {
    const auto& source_manager = context_.getSourceManager();
    auto location = statement->getSwitchLoc();

    if (!source_manager.isInMainFile(location)) {
      return true;
    }

    if (!source_manager.isMacroBodyExpansion(location)) {
      clang::Token token;
      const auto& lang_opts = context_.getLangOpts();
      clang::Lexer::getRawToken(location, token, source_manager, lang_opts);

      auto line = source_manager.getSpellingLineNumber(location);
      auto column = source_manager.getSpellingColumnNumber(location);
      visitor_(line, column, token.getLength(), index::ColorScheme::KEYWORD);
    }

    return true;
  }

  bool VisitWhileStmt(clang::WhileStmt* statement) {
    const auto& source_manager = context_.getSourceManager();
    auto location = statement->getWhileLoc();

    if (!source_manager.isInMainFile(location)) {
      return true;
    }

    if (!source_manager.isMacroBodyExpansion(location)) {
      clang::Token token;
      const auto& lang_opts = context_.getLangOpts();
      clang::Lexer::getRawToken(location, token, source_manager, lang_opts);

      auto line = source_manager.getSpellingLineNumber(location);
      auto column = source_manager.getSpellingColumnNumber(location);
      visitor_(line, column, token.getLength(), index::ColorScheme::KEYWORD);
    }

    return true;
  }

 private:
  index::ClangParser::VisitorFn visitor_;
  const clang::ASTContext& context_;
};

}  // namespace

namespace index {

ClangParser::ClangParser(const std::vector<std::string>& args)
    : unit_(clang::tooling::buildASTFromCodeWithArgs("", args, "")) {
  // TODO: check |unit_| is not |nullptr|.
}

void ClangParser::Visit(VisitorFn visitor) {
  const auto& context = unit_->getASTContext();
  Visitor(visitor, context).TraverseDecl(context.getTranslationUnitDecl());
}

}  // namespace index
}  // namespace ide
