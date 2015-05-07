#pragma once

#include <third_party/ninja/exported/src/state.h>

#include <QStringList>

class Ninja {
 public:
  explicit Ninja(const QString& build_dir);
  ~Ninja();

  inline const QString& build_dir() const { return build_dir_; }

  QStringList QueryAllInputs(const std::string& target);

 private:
  const QString build_dir_, previous_dir_;
  State state_;
};
