#pragma once

#include <project/ninja_project.h>

#include <QMenu>
#include <QTreeWidget>

#include <memory>

class ProjectTree : public QTreeWidget {
  Q_OBJECT

 public:
  explicit ProjectTree(QWidget* parent = nullptr);

  void OpenProject(ide::NinjaProject* project);
  void SwitchVariant(unsigned index);
  void CloseProject();

 public slots:
  void AddNewFile();
  void AddExistingFile();
  void RemoveFile();

 private:
  void ShowFile(const QString& rootless_file_name, bool temporary);

  std::unique_ptr<ide::NinjaProject> project_;
};
