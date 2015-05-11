#include <project/ninja_project.h>

#include <third_party/ninja/ninja.h>

#include <QDir>
#include <QFile>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

namespace ide {

NinjaProject::Iterator::Iterator(PersistentFiles::const_iterator current,
                                 PersistentFiles::const_iterator end,
                                 TemporaryFiles::const_iterator temp_begin,
                                 TemporaryFiles::const_iterator temp_end)
    : pers_begin_(current),
      pers_end_(end),
      temp_begin_(temp_begin),
      temp_end_(temp_end),
      temp_(false) {
  if (current == end) {
    temp_ = true;
  }
}

NinjaProject::Iterator::Iterator(TemporaryFiles::const_iterator current,
                                 TemporaryFiles::const_iterator end)
    : temp_begin_(current), temp_end_(end), temp_(true) {
}

NinjaProject::Iterator& NinjaProject::Iterator::operator++() {
  if (pers_begin_ != pers_end_) {
    ++pers_begin_;
    if (pers_begin_ == pers_end_) {
      temp_ = true;
    }
  } else {
    ++temp_begin_;
  }

  return *this;
}

const String& NinjaProject::Iterator::operator*() const {
  if (temp_) {
    return temp_begin_.key();
  } else {
    return pers_begin_.key();
  }
}

bool NinjaProject::Iterator::operator!=(const Iterator& other) const {
  if (temp_ != other.temp_) {
    return true;
  }

  if (temp_) {
    return temp_begin_ != other.temp_begin_;
  } else {
    return pers_begin_ != other.pers_begin_;
  }
}

StringList NinjaProject::Iterator::GetCommand() const {
  if (temp_) {
    return temp_begin_.value();
  }

  return {};
}

NinjaProject::NinjaProject(const String& project_file, ui32 default_variant)
    : config_path_(project_file) {
  QFile file(project_file);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw std::runtime_error("Failed to open file!");
  }

  auto contents = file.readAll().toStdString();
  if (!google::protobuf::TextFormat::ParseFromString(contents, &config_)) {
    throw std::runtime_error("Project file is malformed!");
  }

  root_path_ = StdToStr(config_.root());
  if (root_path_[0] != '/') {
    root_path_ = QDir::cleanPath(QFileInfo(project_file).absoluteDir().path() +
                                 QDir::separator() + root_path_);
  }

  for (auto i = 0, s = config_.file_size(); i < s; ++i) {
    persistent_files_.insert(StdToStr(config_.file(i)), i);
    // TODO: check that all files are within root path.
  }

  if (config_.variant().empty()) {
    return;
  }

  SwitchVariant(default_variant);
}

NinjaProject::Iterator NinjaProject::AddFile(String file_path) {
  Q_ASSERT(QDir::isAbsolutePath(file_path));
  Q_ASSERT(QFile::exists(file_path));

  file_path = QDir::cleanPath(file_path);
  if (!file_path.startsWith(root_path_)) {
    return end();
  }

  // Truncate leading |root_path_|.
  file_path = file_path.mid(root_path_.size() + 1);

  if (temporary_files_.contains(file_path) ||
      persistent_files_.contains(file_path)) {
    return end();
  }

  config_.add_file()->assign(file_path.toStdString());
  auto it = persistent_files_.insert(file_path, config_.file_size() - 1);
  FlushOnDisk();

  return Iterator(it, persistent_files_.end(), temporary_files_.begin(),
                  temporary_files_.end());
}

void NinjaProject::RemoveFile(const String& file_path) {
  auto it = persistent_files_.find(file_path);
  Q_ASSERT(it != persistent_files_.end());
  RemoveFile(it.value());
}

void NinjaProject::SwitchVariant(ui32 index) {
  if (index >= static_cast<ui32>(config_.variant_size())) {
    throw std::runtime_error("Trying to switch to non-existent variant!");
  }

  const auto& config = config_.variant(index);
  auto build_dir = GetRoot() + QDir::separator() + StdToStr(config.build_dir());
  Ninja ninja(build_dir);
  QDir::setCurrent(build_dir);

  auto inputs = ninja.QueryAllInputs(config.target());
  for (const auto& input : inputs) {
    // We need to filter out the generated files.
    if (QFile::exists(input.first)) {
      AddTemporaryFile(input.first, input.second);
    }
  }
}

bool NinjaProject::AddTemporaryFile(String file_path,
                                    const StringList& command) {
  Q_ASSERT(QDir::isAbsolutePath(file_path));
  Q_ASSERT(QFile::exists(file_path));

  file_path = QDir::cleanPath(file_path);
  if (!file_path.startsWith(root_path_)) {
    return false;
  }

  // Truncate leading |root_path_|.
  file_path = file_path.mid(root_path_.size() + 1);
  auto it = persistent_files_.find(file_path);
  if (it != persistent_files_.end()) {
    persistent_files_.erase(it);
    RemoveFile(it.value());
  }

  if (temporary_files_.contains(file_path)) {
    return false;
  }

  temporary_files_.insert(file_path, command);

  return true;
}

void NinjaProject::RemoveFile(int file_index) {
  Q_ASSERT(file_index > -1 && file_index < config_.file_size());
  config_.mutable_file()->SwapElements(file_index, config_.file_size() - 1);
  config_.mutable_file()->RemoveLast();
  FlushOnDisk();
}

void NinjaProject::FlushOnDisk() const {
  QFile file(config_path_ + ".save");
  file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
  google::protobuf::io::FileOutputStream out(file.handle());
  google::protobuf::TextFormat::Print(config_, &out);
  out.Close();

  QDir dir;
  dir.remove(config_path_);
  dir.rename(config_path_ + ".save", config_path_);
}

}  // namespace ide
