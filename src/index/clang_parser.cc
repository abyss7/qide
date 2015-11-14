#include <index/clang_parser.h>

#include <iostream>

namespace ide {
namespace index {

ClangParser::ClangParser() : index_(clang_createIndex(1, 1)) {
  clang_CXIndex_setGlobalOptions(
      index_, clang_CXIndex_getGlobalOptions(index_) |
                  CXGlobalOpt_ThreadBackgroundPriorityForAll);
}

ClangParser::~ClangParser() {
  for (const auto& unit : units_) {
    clang_disposeTranslationUnit(unit);
  }
  clang_disposeIndex(index_);
}

void ClangParser::Index(const AbsolutePath& path, const StringVector& args,
                        const AbsolutePath& args_dir) {
  Parse(path, args, args_dir);
  auto unit = units_.find(path);
  if (unit != units_.end()) {
    clang_disposeTranslationUnit(*unit);
    units_.erase(unit);
  }
}

void ClangParser::Complete(const AbsolutePath& file_path,
                           const StringVector& args,
                           const AbsolutePath& args_dir, ui32 line,
                           ui32 column) {
  auto unit = units_.find(file_path);
  if (unit == units_.end()) {
    if (!Parse(file_path, args, args_dir)) {
      std::cout << "Unit not found!" << std::endl;
      return;
    }
    unit = units_.find(file_path);
  }

  auto old_cwd = QDir::currentPath();
  QDir::setCurrent(args_dir);
  auto* results = clang_codeCompleteAt(
      *unit, file_path.str.toStdString().c_str(), line, column, nullptr, 0,
      clang_defaultCodeCompleteOptions());
  QDir::setCurrent(old_cwd);
  if (!results) {
    std::cout << "clang_codeCompleteAt failed!" << std::endl;
    return;
  }

  unsigned numDiags = clang_codeCompleteGetNumDiagnostics(results);
  std::cout << "debug: complete: " << numDiags << " diagnostic(s)" << std::endl;
  for (unsigned i = 0; i < numDiags; ++i) {
    CXDiagnostic diagnostic = clang_codeCompleteGetDiagnostic(results, i);
    CXString s = clang_formatDiagnostic(
        diagnostic, clang_defaultDiagnosticDisplayOptions());

    std::cout << clang_getCString(s) << std::endl;
    clang_disposeString(s);
    clang_disposeDiagnostic(diagnostic);
  }

  clang_sortCodeCompletionResults(results->Results, results->NumResults);

  for (auto i = 0u; i < results->NumResults; ++i) {
    auto cc = results->Results[i].CompletionString;

    auto avail = clang_getCompletionAvailability(cc);
    if (avail == CXAvailability_NotAccessible ||
        avail == CXAvailability_NotAvailable) {
      continue;
    }

    auto chunks_size = clang_getNumCompletionChunks(cc);
    for (auto j = 0u; j < chunks_size; ++j) {
      auto str = clang_getCompletionChunkText(cc, j);
      std::cout << clang_getCString(str) << " ";
      clang_disposeString(str);
    }
    std::cout << std::endl;
  }

  clang_disposeCodeCompleteResults(results);
}

bool ClangParser::Parse(const AbsolutePath& path, const StringVector& args,
                        const AbsolutePath& args_dir) {
  std::vector<const char*> tmp;
  tmp.reserve(args.size() + 1);
  for (auto i = 1u; i < args.size(); ++i) {
    tmp[i - 1] = args[i].c_str();
  }

  tmp[args.size() - 1] = "-isystem";
  tmp[args.size()] =
      "/home/ilezhankin/.local/opt/llvm.47.0.2526/lib/clang/3.8.0/include";

  auto options = clang_defaultEditingTranslationUnitOptions();
  options |= CXTranslationUnit_PrecompiledPreamble;
  // FIXME: does it really not work?
  // options &= ~CXTranslationUnit_CacheCompletionResults;
  options |= CXTranslationUnit_DetailedPreprocessingRecord;

  auto old_cwd = QDir::currentPath();
  QDir::setCurrent(args_dir);
  auto unit = clang_parseTranslationUnit(index_, nullptr, tmp.data(),
                                         args.size() + 1, nullptr, 0, options);
  clang_reparseTranslationUnit(unit, 0, nullptr,
                               clang_defaultReparseOptions(unit));
  QDir::setCurrent(old_cwd);
  if (!unit) {
    return false;
  }

  auto old_unit = units_.find(path);
  if (old_unit != units_.end()) {
    clang_disposeTranslationUnit(*old_unit);
    units_.erase(old_unit);
  }
  units_.insert(path, unit);

  return true;
}

}  // namespace index
}  // namespace ide
