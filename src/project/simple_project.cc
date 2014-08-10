#include <project/simple_project.h>

#include <QDir>
#include <QFile>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

namespace ide {

SimpleProject::SimpleProject(const QString &project_path)
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

  if (!project_.has_root()) {
    root_path_ = ".";
  } else {
    root_path_ = QString::fromStdString(project_.root());
  }
  if (root_path_[0] != '/') {
    root_path_ = QDir::cleanPath(QFileInfo(proto_path_).absoluteDir().path() +
                                 QDir::separator() + root_path_);
  }

  // TODO: check that all files are within root path.
}

bool SimpleProject::AddFile(QString &file_path) {
  Q_ASSERT(QDir::isAbsolutePath(file_path));
  Q_ASSERT(QFile::exists(file_path));

  auto clean_file_path = QDir::cleanPath(file_path);
  if (!clean_file_path.startsWith(root_path_)) {
    return false;
  }

  file_path = clean_file_path.mid(root_path_.size() + 1);

  if (FindFile(file_path) != -1) {
    return false;
  }

  project_.add_file()->assign(file_path.toStdString());
  FlushOnDisk();

  return true;
}

void SimpleProject::RemoveFile(const QString &file_path) {
  int index = FindFile(file_path);
  if (index != -1) {
    project_.mutable_file()->SwapElements(index, project_.file_size() - 1);
    project_.mutable_file()->RemoveLast();
    FlushOnDisk();
  }
}

int SimpleProject::FindFile(const QString &file_path) const {
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
