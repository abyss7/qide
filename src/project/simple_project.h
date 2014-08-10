#pragma once

#include <project/base_project.h>
#include <project/project.pb.h>

namespace ide {

class SimpleProject : public BaseProject {
 public:
  explicit SimpleProject(const QString& project_path);
  virtual ~SimpleProject() {}

  virtual QString GetName() const override {
    return QString::fromStdString(project_.name());
  }
  virtual const QString& GetRoot() const override { return root_path_; }
  virtual bool AddFile(QString& file_path) override;
  virtual void RemoveFile(const QString& file_path) override;
  virtual FileIterator begin() const override {
    return FileIterator(project_.file().begin());
  }
  virtual FileIterator end() const override {
    return FileIterator(project_.file().end());
  }

 private:
  int FindFile(const QString& file_path) const;
  void FlushOnDisk() const;

  const QString proto_path_;
  QString root_path_;
  proto::Project project_;
};

}  // namespace ide
