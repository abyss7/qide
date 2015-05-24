#include <index/clang_parser.h>

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Tooling/Tooling.h>

namespace ide {

namespace {

class Visitor : public clang::RecursiveASTVisitor<Visitor> {
 public:
  Visitor(index::ClangParser::VisitorFn visitor) : visitor_(visitor) {}

  bool VisitCXXRecordDecl(clang::CXXRecordDecl* decl) {
    decl->dump();
    return true;
  }

 private:
  index::ClangParser::VisitorFn visitor_;
};

}  // namespace

namespace index {

ClangParser::ClangParser(const std::vector<std::string>& args)
    : unit_(clang::tooling::buildASTFromCodeWithArgs("", args, "")) {
  // TODO: check |unit_| is not |nullptr|.
}

void ClangParser::Visit(VisitorFn visitor) {
  Visitor(visitor)
      .TraverseDecl(unit_->getASTContext().getTranslationUnitDecl());
}

}  // namespace index
}  // namespace ide
