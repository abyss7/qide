#pragma once

#include <base/path.h>

namespace ide {

class Project {
 protected:
  struct IteratorBase {
    virtual ~IteratorBase() {}
    virtual IteratorBase* clone() const = 0;

    virtual IteratorBase& operator++() = 0;
    virtual bool operator!=(const IteratorBase& other) const = 0;

    virtual RelativePath path() const = 0;
    virtual StringList args() const = 0;
  };

 public:
  class Iterator {
   public:
    Iterator(UniquePtr<IteratorBase> impl) : impl_(std::move(impl)) {}
    Iterator(const Iterator& other) : impl_(other.impl_->clone()) {}

    inline Iterator& operator++() {
      ++(*impl_);
      return *this;
    }

    inline bool operator!=(const Iterator& other) const {
      return *impl_ != *other.impl_;
    }

    inline RelativePath path() const { return impl_->path(); }
    inline StringList args() const { return impl_->args(); }

   private:
    UniquePtr<IteratorBase> impl_;
  };

  virtual ~Project() {}

  virtual String name() const = 0;
  virtual AbsolutePath root() const = 0;

  virtual ui32 size() const = 0;
  // Get number of files to track the progress while the project is loading.

  // Iterate over all files.
  virtual Iterator begin() const = 0;
  virtual Iterator end() const = 0;
};

}  // namespace ide
