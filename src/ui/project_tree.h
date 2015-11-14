#pragma once

#include <project/base_project.h>

#include <QProgressBar>
#include <QTreeWidget>

namespace ide {

class NinjaProject;

namespace ui {

class ProjectTree : public QTreeWidget {
  Q_OBJECT

 public:
  explicit ProjectTree(QWidget* parent = nullptr);
  ~ProjectTree();

  void OpenProject(NinjaProject* project, QProgressBar* progress);
  void SwitchVariant(ui32 index, QProgressBar* progress);
  void CloseProject();

 public slots:
  void AddNewFile();
  void AddExistingFile();
  void RemoveFile();

 private:
  void Populate(QProgressBar* progress);
  void ShowFile(Project::Iterator file);

  NinjaProject* project_ = nullptr;
};

}  // namespace ui
}  // namespace ide
