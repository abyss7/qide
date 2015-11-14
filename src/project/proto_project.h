#pragma once

#include <project/base_project.h>
#include <project/project.pb.h>

namespace ide {

/*
 * This project stores information in a protobuf text format.
 *
 * It allows to store multiple variants of the same project, e.g. build
 * configurations like Debug, Release, etc - in a separate folders.
 *
 * Also, it allows to store additional files that are presented in all variants
 * and are not used for compilation. e.g. configuration files, scripts, etc.
 */
class ProtoProject : public Project {
 public:
  explicit ProtoProject(const AbsolutePath& project_path);

  inline String name() const override { return StdToStr(config_.name()); }
  inline AbsolutePath root() const override { return AbsolutePath(root_); }

  inline ui32 size() const override { return config_.file_size(); }

  Iterator begin() const override;
  Iterator end() const override;

  Iterator AppendFile(const AbsolutePath& path);
  // Returns |end()| if file is already appended.

  bool RemoveFile(const RelativePath& path);
  // The |path| must be relative to |root()|.
  // Doesn't affect the real file on file system.
  // Returns false if file was never appended with |AppendFile()|.

  inline ui32 variant() const { return variant_; }
  inline ui32 variant_size() const { return config_.variant_size(); }
  inline String variant_name(ui32 index) const {
    return StdToStr(config_.variant(index).name());
  }
  bool switch_variant(ui32 index);

 protected:
  virtual bool switch_variant_impl(const AbsolutePath& build_dir,
                                   const String& target) = 0;

  inline const proto::Project& config() const { return config_; }

 private:
  class IteratorImpl : public IteratorBase {
   public:
    using FileIterator =
        decltype(std::declval<proto::Project>().file().begin());

    IteratorImpl(FileIterator it);
    IteratorBase* clone() const override;

    IteratorBase& operator++() override;
    bool operator!=(const IteratorBase& other) const override;

    RelativePath path() const override;
    StringList args() const override { return StringList(); }

   private:
    FileIterator it_;
  };

  void flush() const;

  const String config_path_;
  proto::Project config_;
  ui32 variant_ = -1;
  String root_;
};

}  // namespace ide
