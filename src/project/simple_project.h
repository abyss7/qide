#pragma once

#include <project/base_project.h>
#include <project/project.pb.h>

namespace ide {

class SimpleProject : public BaseProject {
 public:
  static SimpleProject* New(const QString& name, const QString& project_path);

  explicit SimpleProject(const QString& project_path);

  inline QString GetName() const override {
    return QString::fromStdString(project_.name());
  }
  inline const QString& GetRoot() const override { return root_path_; }
  bool AddFile(QString& file_path) override;
  void RemoveFile(const QString& file_path) override;
  inline FileIterator begin() const override {
    return FileIterator(project_.file().begin());
  }
  inline FileIterator end() const override {
    return FileIterator(project_.file().end());
  }

 protected:
  proto::Project project_;

 private:
  SimpleProject(const QString& name, const QString& project_path);

  int FindFile(const QString& file_path) const;
  void FlushOnDisk() const;

  const QString proto_path_;
  QString root_path_;
};

}  // namespace ide
