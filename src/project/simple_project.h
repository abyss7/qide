#pragma once

#include <project/file_iterator.h>

#include <QSet>
#include <QString>

namespace ide {

class SimpleProject {
 public:
  static SimpleProject* New(const QString& name, const QString& project_path);

  explicit SimpleProject(const QString& project_path);

  inline QString GetName() const {
    return QString::fromStdString(project_.name());
  }
  inline const QString& GetRoot() const { return root_path_; }

  bool AddFile(QString& file_path, bool temporary);
  void RemoveFile(const QString& file_path);

  inline FileIterator begin() const {
    return FileIterator(project_.file().begin());
  }
  inline FileIterator end() const {
    return FileIterator(project_.file().end());
  }

  inline QSet<QString>::const_iterator temporary_begin() const {
    return temporary_files_.begin();
  }
  inline QSet<QString>::const_iterator temporary_end() const {
    return temporary_files_.end();
  }

 protected:
  void RemoveFile(int file_index);

  proto::Project project_;
  QSet<QString> temporary_files_;

 private:
  SimpleProject(const QString& name, const QString& project_path);

  int FindFile(const QString& file_path) const;
  void FlushOnDisk() const;

  const QString proto_path_;
  QString root_path_;
};

}  // namespace ide
