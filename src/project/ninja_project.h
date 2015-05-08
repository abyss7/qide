#pragma once

#include <project/simple_project.h>

namespace ide {

class NinjaProject : public SimpleProject {
 public:
  explicit NinjaProject(const QString& project_path,
                        unsigned default_config = 0);

  inline unsigned ConfigurationSize() const { return project_.config_size(); }
  inline QString GetConfigurationName(unsigned index) const {
    return QString::fromStdString(project_.config(index).name());
  }
  inline unsigned GetCurrentConfiguration() const { return current_config_; }
  void SwitchConfiguration(unsigned index);

 private:
  unsigned current_config_ = 0;
};

}  // namespace ide
