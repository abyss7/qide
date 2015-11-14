#include <project/ninja_project.h>

#include <QProcess>
#include <QTextStream>

namespace ide {

NinjaProject::NinjaProject(const AbsolutePath& project_path)
    : ProtoProject(project_path) {}

ui32 NinjaProject::size() const {
  Q_ASSERT(compdb_project_);

  return ProtoProject::size() + compdb_project_->size();
}

Project::Iterator NinjaProject::begin() const {
  Q_ASSERT(compdb_project_);
  return UniquePtr<IteratorBase>(new IteratorImpl(
      ProtoProject::begin(), ProtoProject::end(), compdb_project_->begin()));
}

Project::Iterator NinjaProject::end() const {
  Q_ASSERT(compdb_project_);
  return UniquePtr<IteratorBase>(new IteratorImpl(compdb_project_->end()));
}

bool NinjaProject::switch_variant_impl(const AbsolutePath& build_dir,
                                       const String& target) {
  QProcess ninja;
  StringList args = {"-C", build_dir, target, "-t", "compdb"};

  for (auto it = config().rules().begin(); it != config().rules().end(); ++it) {
    args.push_back(StdToStr(*it));
  }
  ninja.start("ninja", args);
  ninja.waitForFinished();

  if (ninja.exitCode()) {
    return false;
  }

  QFile file(build_dir + QDir::separator() + "compile_commands.json");
  if (file.open(QIODevice::WriteOnly)) {
    QTextStream stream(&file);
    stream << ninja.readAllStandardOutput();
  } else {
    return false;
  }
  file.close();

  compdb_project_.reset(new CompdbProject(root(), build_dir));

  return true;
}

NinjaProject::IteratorImpl::IteratorImpl(Iterator proto_current,
                                         Iterator proto_end,
                                         Iterator compdb_begin)
    : proto_(proto_current != proto_end),
      proto_it_(proto_current),
      proto_end_(proto_end),
      compdb_it_(compdb_begin) {}

NinjaProject::IteratorImpl::IteratorImpl(Iterator compdb_current)
    : proto_(false),
      proto_it_(compdb_current),
      proto_end_(compdb_current),
      compdb_it_(compdb_current) {}

Project::IteratorBase* NinjaProject::IteratorImpl::clone() const {
  if (proto_) {
    return UniquePtr<IteratorBase>(
               new IteratorImpl(proto_it_, proto_end_, compdb_it_))
        .release();
  } else {
    return UniquePtr<IteratorBase>(new IteratorImpl(compdb_it_)).release();
  }
}

Project::IteratorBase& NinjaProject::IteratorImpl::operator++() {
  if (proto_) {
    ++proto_it_;
    if (!(proto_it_ != proto_end_)) {
      proto_ = false;
    }
  } else {
    ++compdb_it_;
  }

  return *this;
}

bool NinjaProject::IteratorImpl::operator!=(const IteratorBase& other) const {
  auto it = static_cast<const IteratorImpl&>(other);

  if (proto_ != it.proto_) {
    return true;
  }

  if (proto_) {
    return proto_it_ != it.proto_it_;
  } else {
    return compdb_it_ != it.compdb_it_;
  }
}

RelativePath NinjaProject::IteratorImpl::path() const {
  if (proto_) {
    return proto_it_.path();
  } else {
    return compdb_it_.path();
  }
}

StringVector NinjaProject::IteratorImpl::args() const {
  if (proto_) {
    return proto_it_.args();
  } else {
    return compdb_it_.args();
  }
}

AbsolutePath NinjaProject::IteratorImpl::args_dir() const {
  if (proto_) {
    return proto_it_.args_dir();
  } else {
    return compdb_it_.args_dir();
  }
}

bool NinjaProject::IteratorImpl::has_args() const {
  if (proto_) {
    return proto_it_.has_args();
  } else {
    return compdb_it_.has_args();
  }
}

}  // namespace ide
