#pragma once

#include <project/simple_project.h>

namespace ide {

class NinjaProject : public SimpleProject {
 public:
  explicit NinjaProject(const QString& project_path,
                        unsigned default_config = 0);

  void SwitchConfiguration(unsigned index);

 private:
  unsigned current_config_ = 0;
};

}  // namespace ide
