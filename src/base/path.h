#pragma once

#include <base/alias.h>

#include <QDir>

namespace ide {

struct RelativePath {
  explicit RelativePath(const String& path) : str(path) {
    Q_ASSERT(QDir::isRelativePath(path));
  }

  inline operator String() const { return str; }
  const String str;
};

struct AbsolutePath {
  explicit AbsolutePath(const String& path) : str(path) {
    Q_ASSERT(QDir::isAbsolutePath(path));
  }

  inline operator String() const { return str; }
  const String str;
};

}  // namespace ide
