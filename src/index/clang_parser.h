#pragma once

#include <base/path.h>

#include <clang-c/Index.h>

namespace ide {
namespace index {

class ClangParser {
 public:
  ClangParser();
  ~ClangParser();

  void Index(const AbsolutePath& path, const StringVector& args,
             const AbsolutePath& args_dir);
  void Complete(const AbsolutePath& file_path, const StringVector& args,
                const AbsolutePath& args_dir, ui32 line, ui32 column);

 private:
  bool Parse(const AbsolutePath& path, const StringVector& args,
             const AbsolutePath& args_dir);

  CXIndex index_;
  HashMap<String, CXTranslationUnit> units_;
};

}  // namespace index
}  // namespace ide
