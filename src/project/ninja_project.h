#pragma once

#include <base/alias.h>
#include <project/project.pb.h>

#include <clang-c/Index.h>

namespace ide {

class NinjaProject {
 public:
  using PersistentFiles = HashMap<String, int>;
  using TemporaryFiles = HashMap<String, StringList>;

  class Iterator {
   public:
    Iterator(PersistentFiles::const_iterator current,
             PersistentFiles::const_iterator end,
             TemporaryFiles::const_iterator temp_begin,
             TemporaryFiles::const_iterator temp_end);

    Iterator(TemporaryFiles::const_iterator current,
             TemporaryFiles::const_iterator end);

    Iterator& operator++();
    const String& operator*() const;
    bool operator!=(const Iterator& other) const;

    inline bool IsTemporary() const { return temp_; }
    StringList GetCommand() const;

   private:
    PersistentFiles::const_iterator pers_begin_, pers_end_;
    TemporaryFiles::const_iterator temp_begin_, temp_end_;
    bool temp_;
  };

  explicit NinjaProject(const String& project_file, ui32 default_variant = 0);
  ~NinjaProject();

  inline String GetName() const { return StdToStr(config_.name()); }
  inline const String& GetRoot() const { return root_path_; }
  inline CXIndex GetIndex() { return index_; }
  inline ui32 GetFileCount() const {
    return persistent_files_.size() + temporary_files_.size();
  }

  Iterator AddFile(String file_path);
  void RemoveFile(const String& file_path);

  inline Iterator begin() const {
    return Iterator(persistent_files_.begin(), persistent_files_.end(),
                    temporary_files_.begin(), temporary_files_.end());
  }
  inline Iterator end() const {
    return Iterator(temporary_files_.end(), temporary_files_.end());
  }

  inline ui32 CurrentVariant() const { return current_variant_; }
  inline ui32 VariantSize() const { return config_.variant_size(); }
  inline String GetVariantName(ui32 index) const {
    return StdToStr(config_.variant(index).name());
  }
  void SwitchVariant(ui32 index);

 private:
  bool AddTemporaryFile(String file_path, const StringList& command);
  void RemoveFile(int file_index);
  void FlushOnDisk() const;

  const String config_path_;
  proto::Project config_;
  PersistentFiles persistent_files_;
  TemporaryFiles temporary_files_;
  String root_path_;
  ui32 current_variant_ = -1;
  CXIndex index_ = clang_createIndex(1, 1);
};

}  // namespace ide
