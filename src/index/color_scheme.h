#pragma once

#include <base/alias.h>

#include <QColor>

namespace ide {
namespace index {

class ColorScheme {
  struct Color {
    QColor fg, bg;
  };

 public:
  enum Kind {
    DEFAULT = 0,

    // UI
    LINE_NUMBER,
    HIGHLIGHT_LINE,

    // Tokens
    COMMENT,
    KEYWORD,
    NAMESPACE,
    NUMBER_LITERAL,
    STRING_LITERAL,
  };

  ColorScheme(QColor default_fg, QColor default_bg);

  Color& operator[](Kind kind);

 private:
  HashMap<Kind, Color> scheme_;
};

}  // namespace index
}  // namespace ide
