#include <project/proto_project.h>

#include <third_party/protobuf/exported/src/google/protobuf/io/zero_copy_stream_impl.h>
#include <third_party/protobuf/exported/src/google/protobuf/text_format.h>

namespace ide {

ProtoProject::ProtoProject(const AbsolutePath& project_path)
    : config_path_(project_path) {
  QFile file(project_path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw std::runtime_error("Failed to open file!");
  }

  auto contents = file.readAll().toStdString();
  if (!google::protobuf::TextFormat::ParseFromString(contents, &config_)) {
    throw std::runtime_error("Project file is malformed!");
  }

  root_ = StdToStr(config_.root());
  if (root_[0] != '/') {
    root_ = QDir::cleanPath(QFileInfo(project_path).absoluteDir().path() +
                            QDir::separator() + root_);
  }

  // TODO: check that all files are within root path.
}

Project::Iterator ProtoProject::begin() const {
  return UniquePtr<IteratorBase>(new IteratorImpl(config_.file().begin()));
}
Project::Iterator ProtoProject::end() const {
  return UniquePtr<IteratorBase>(new IteratorImpl(config_.file().end()));
}

Project::Iterator ProtoProject::AppendFile(const AbsolutePath& path) {
  auto clean_path = QDir::cleanPath(path);
  if (!clean_path.startsWith(root_)) {
    return end();
  }

  // Truncate leading |root_|.
  clean_path = clean_path.mid(root_.size() + 1);

  auto clean_path_str = clean_path.toStdString();
  for (auto it = config_.file().begin(); it != config_.file().end(); ++it) {
    if (*it == clean_path_str) {
      return end();
    }
  }

  config_.add_file()->assign(clean_path_str);
  flush();

  return Iterator(
      UniquePtr<IteratorBase>(new IteratorImpl(config_.file().end() - 1)));
}

bool ProtoProject::RemoveFile(const RelativePath& path) {
  auto path_str = path.str.toStdString();
  for (auto i = 0; i < config_.file_size(); ++i) {
    if (config_.file(i) == path_str) {
      config_.mutable_file()->SwapElements(i, config_.file_size() - 1);
      config_.mutable_file()->RemoveLast();
      flush();
      return true;
    }
  }

  return false;
}

bool ProtoProject::switch_variant(ui32 index) {
  if (index >= variant_size()) {
    return false;
  }

  const auto& config = config_.variant(index);
  AbsolutePath build_dir(root() + QDir::separator() +
                         StdToStr(config.build_dir()));

  if (!switch_variant_impl(build_dir, StdToStr(config.target()))) {
    return false;
  }

  variant_ = index;

  return true;
}

void ProtoProject::flush() const {
  QFile file(config_path_ + ".save");
  file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
  google::protobuf::io::FileOutputStream out(file.handle());
  google::protobuf::TextFormat::Print(config_, &out);
  out.Close();

  QDir dir;
  dir.remove(config_path_);
  dir.rename(config_path_ + ".save", config_path_);
}

ProtoProject::IteratorImpl::IteratorImpl(FileIterator it) : it_(it) {}

Project::IteratorBase* ProtoProject::IteratorImpl::clone() const {
  return UniquePtr<IteratorBase>(new IteratorImpl(it_)).release();
}

Project::IteratorBase& ProtoProject::IteratorImpl::operator++() {
  ++it_;
  return *this;
}

bool ProtoProject::IteratorImpl::operator!=(const IteratorBase& other) const {
  auto it = static_cast<const IteratorImpl&>(other);
  return it_ != it.it_;
}

RelativePath ProtoProject::IteratorImpl::path() const {
  return RelativePath(StdToStr(*it_));
}

}  // namespace ide
