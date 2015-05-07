#include <project/simple_project.h>

#include <QDir>
#include <QFile>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

namespace ide {

// static
SimpleProject* SimpleProject::New(const QString& name,
                                  const QString& project_path) {
  std::unique_ptr<SimpleProject> new_project(
      new SimpleProject(name, project_path));
  new_project->FlushOnDisk();

  return new_project.release();
}

SimpleProject::SimpleProject(const QString& project_path)
    : proto_path_(project_path) {
  QFile file(proto_path_);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw std::runtime_error("Failed to open file!");
  }

  auto contents = file.readAll();
  auto str = std::string(contents.data(), contents.size());
  if (!google::protobuf::TextFormat::ParseFromString(str, &project_)) {
    throw std::runtime_error("Project file is malformed!");
  }

  root_path_ = QString::fromStdString(project_.root());
  if (root_path_[0] != '/') {
    root_path_ = QDir::cleanPath(QFileInfo(proto_path_).absoluteDir().path() +
                                 QDir::separator() + root_path_);
  }

  // TODO: check that all files are within root path.
}

bool SimpleProject::AddFile(QString& file_path, bool temporary) {
  Q_ASSERT(QDir::isAbsolutePath(file_path));
  Q_ASSERT(QFile::exists(file_path));

  auto clean_file_path = QDir::cleanPath(file_path);
  if (!clean_file_path.startsWith(root_path_)) {
    return false;
  }

  file_path = clean_file_path.mid(root_path_.size() + 1);

  if (temporary_files_.contains(file_path) || FindFile(file_path) != -1) {
    return false;
  }

  if (!temporary) {
    project_.add_file()->assign(file_path.toStdString());
    FlushOnDisk();
  } else {
    temporary_files_.insert(file_path);
  }

  return true;
}

void SimpleProject::RemoveFile(const QString& file_path) {
  RemoveFile(FindFile(file_path));
}

void SimpleProject::RemoveFile(int file_index) {
  if (file_index > -1 && file_index < project_.file_size()) {
    project_.mutable_file()->SwapElements(file_index, project_.file_size() - 1);
    project_.mutable_file()->RemoveLast();
    FlushOnDisk();
  }
}

SimpleProject::SimpleProject(const QString& name, const QString& project_path)
    : proto_path_(project_path),
      root_path_(QDir::cleanPath(QFileInfo(proto_path_).absoluteDir().path())) {
  project_.set_name(name.toStdString());
}

int SimpleProject::FindFile(const QString& file_path) const {
  for (int i = 0; i < project_.file_size(); ++i) {
    auto str = QString::fromStdString(project_.file(i));
    if (file_path == str || file_path == root_path_ + QDir::separator() + str) {
      return i;
    }
  }

  return -1;
}

void SimpleProject::FlushOnDisk() const {
  QFile file(proto_path_ + ".save");
  file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
  google::protobuf::io::FileOutputStream out(file.handle());
  google::protobuf::TextFormat::Print(project_, &out);
  out.Close();

  QDir dir;
  dir.remove(proto_path_);
  dir.rename(proto_path_ + ".save", proto_path_);
}

}  // namespace ide
