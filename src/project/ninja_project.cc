#include <project/ninja_project.h>

#include <third_party/ninja/ninja.h>

#include <QDir>

namespace ide {

NinjaProject::NinjaProject(const QString& project_name, unsigned default_config)
    : SimpleProject(project_name) {
  if (project_.config_size() == 0) {
    return;
  }

  SwitchConfiguration(default_config);
}

void NinjaProject::SwitchConfiguration(unsigned index) {
  if (index >= static_cast<unsigned>(project_.config_size())) {
    throw std::runtime_error("Trying to switch to non-existent configuration!");
  }
  const auto& config = project_.config(index);
  auto build_dir = GetRoot() + QDir::separator() +
                   QString::fromStdString(config.build_dir());
  Ninja ninja(build_dir);

  auto inputs = ninja.QueryAllInputs(config.target());
  for (auto& input : inputs) {
    // We need to filter out the generated files.
    if (QFile::exists(input)) {
      AddFile(input, true);
    }
  }

  for (auto i = 0; i < project_.file().size(); ++i) {
    if (temporary_files_.contains(QString::fromStdString(project_.file(i)))) {
      RemoveFile(i);
    }
  }

  current_config_ = index;
}

}  // namespace ide
