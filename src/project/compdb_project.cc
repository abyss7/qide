#include <project/compdb_project.h>

namespace ide {

CompdbProject::CompdbProject(const AbsolutePath& root,
                             const AbsolutePath& compdb_dir)
    : root_(root) {
  CXCompilationDatabase_Error error;

  db_ = clang_CompilationDatabase_fromDirectory(
      compdb_dir.str.toStdString().c_str(), &error);
  if (error != CXCompilationDatabase_NoError) {
    throw std::runtime_error("Failed to load compilation database!");
  }

  commands_ = clang_CompilationDatabase_getAllCompileCommands(db_);
}

CompdbProject::~CompdbProject() {
  clang_CompileCommands_dispose(commands_);
  clang_CompilationDatabase_dispose(db_);
}

ui32 CompdbProject::size() const {
  return clang_CompileCommands_getSize(commands_);
}

Project::Iterator CompdbProject::begin() const {
  return UniquePtr<IteratorBase>(new IteratorImpl(commands_, root_));
}

Project::Iterator CompdbProject::end() const {
  return UniquePtr<IteratorBase>(new IteratorImpl(commands_, root_, size()));
}

CompdbProject::IteratorImpl::IteratorImpl(CXCompileCommands commands,
                                          const AbsolutePath& root, ui32 i)
    : commands_(commands), i_(i), root_(root) {}

Project::IteratorBase* CompdbProject::IteratorImpl::clone() const {
  return UniquePtr<IteratorBase>(new IteratorImpl(commands_, root_, i_))
      .release();
}

Project::IteratorBase& CompdbProject::IteratorImpl::operator++() {
  ++i_;
  return *this;
}

bool CompdbProject::IteratorImpl::operator!=(const IteratorBase& other) const {
  auto it = static_cast<const IteratorImpl&>(other);
  return commands_ != it.commands_ || i_ != it.i_;
}

RelativePath CompdbProject::IteratorImpl::path() const {
  auto command = clang_CompileCommands_getCommand(commands_, i_);
  auto dir = clang_CompileCommand_getDirectory(command);
  auto file = clang_CompileCommand_getFilename(command);

  String dir_str(clang_getCString(dir));
  String file_str(clang_getCString(file));

  clang_disposeString(dir);
  clang_disposeString(file);

  file_str = QDir::cleanPath(dir_str + QDir::separator() + file_str);
  file_str = file_str.mid(root_.str.size() + 1);

  return RelativePath(file_str);
}

StringList CompdbProject::IteratorImpl::args() const {
  StringList result;

  auto command = clang_CompileCommands_getCommand(commands_, i_);
  auto args_size = clang_CompileCommand_getNumArgs(command);

  for (ui32 i = 0; i < args_size; ++i) {
    auto arg = clang_CompileCommand_getArg(command, i);
    result.push_back(clang_getCString(arg));
    clang_disposeString(arg);
  }

  return result;
}

}  // namespace ide
