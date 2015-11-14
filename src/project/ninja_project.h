#pragma once

#include <project/compdb_project.h>
#include <project/proto_project.h>

namespace ide {

class NinjaProject : public ProtoProject {
 public:
  explicit NinjaProject(const AbsolutePath& project_path);

  ui32 size() const override;

  Iterator begin() const override;
  Iterator end() const override;

 private:
  class IteratorImpl : public IteratorBase {
   public:
    IteratorImpl(Iterator proto_current, Iterator proto_end,
                 Iterator compdb_begin);
    IteratorImpl(Iterator compdb_current);
    IteratorBase* clone() const override;

    IteratorBase& operator++() override;
    bool operator!=(const IteratorBase& other) const override;

    RelativePath path() const override;
    StringList args() const override;

   private:
    bool proto_;
    Iterator proto_it_, proto_end_, compdb_it_;
  };

  bool switch_variant_impl(const AbsolutePath& build_dir,
                           const String& target) override;

  UniquePtr<CompdbProject> compdb_project_;
};

}  // namespace ide
