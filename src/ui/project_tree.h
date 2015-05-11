#pragma once

#include <project/ninja_project.h>

#include <QMenu>
#include <QTreeWidget>

namespace ide {
namespace ui {

class ProjectTree : public QTreeWidget {
  Q_OBJECT

 public:
  explicit ProjectTree(QWidget* parent = nullptr);

  void OpenProject(NinjaProject* project);
  void SwitchVariant(ui32 index);
  void CloseProject();

 public slots:
  void AddNewFile();
  void AddExistingFile();
  void RemoveFile();

 private:
  void ShowFile(NinjaProject::Iterator file);

  UniquePtr<NinjaProject> project_;
};

}  // namespace ui
}  // namespace ide
