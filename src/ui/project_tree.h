#pragma once

#include <project/ninja_project.h>

#include <QMenu>
#include <QProgressBar>
#include <QTreeWidget>

namespace ide {
namespace ui {

class ProjectTree : public QTreeWidget {
  Q_OBJECT

 public:
  explicit ProjectTree(QWidget* parent = nullptr);

  void OpenProject(NinjaProject* project, QProgressBar* progress);
  void SwitchVariant(ui32 index, QProgressBar* progress);
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
