#pragma once

#include <third_party/ninja/exported/src/state.h>

#include <QPair>
#include <QStringList>

class Ninja {
 public:
  using InputList = QList<QPair<QString, QStringList>>;

  explicit Ninja(const QString& build_dir);

  inline const QString& build_dir() const { return build_dir_; }

  InputList QueryAllInputs(const std::string& target, const QStringList& rules);

 private:
  const QString build_dir_;
  State state_;
};
