#pragma once

#include <project/file_iterator.h>

#include <QString>

namespace ide {

class BaseProject {
 public:
  virtual ~BaseProject() {}

  virtual QString GetName() const = 0;
  virtual const QString& GetRoot() const = 0;
  virtual bool AddFile(QString& file_path) = 0;
  virtual void RemoveFile(const QString& file_path) = 0;
  virtual FileIterator begin() const = 0;
  virtual FileIterator end() const = 0;
};

}  // namespace ide
