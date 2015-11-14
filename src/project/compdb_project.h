#pragma once

#include <project/base_project.h>

#include <clang-c/CXCompilationDatabase.h>

namespace ide {

class CompdbProject : public Project {
 public:
  explicit CompdbProject(const AbsolutePath& root,
                         const AbsolutePath& compdb_dir);
  ~CompdbProject() override;

  inline String name() const override { return String(); }
  inline AbsolutePath root() const override { return root_; }

  ui32 size() const override;

  Iterator begin() const override;
  Iterator end() const override;

 private:
  class IteratorImpl : public IteratorBase {
   public:
    explicit IteratorImpl(CXCompileCommands commands, const AbsolutePath& root,
                          ui32 i = 0);
    IteratorBase* clone() const override;

    IteratorBase& operator++() override;
    bool operator!=(const IteratorBase& other) const override;

    RelativePath path() const override;
    StringList args() const override;

   private:
    CXCompileCommands commands_;
    ui32 i_;
    const AbsolutePath& root_;
  };

  CXCompilationDatabase db_;
  CXCompileCommands commands_;
  const AbsolutePath root_;
};

}  // namespace ide
