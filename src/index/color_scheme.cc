#include <index/color_scheme.h>

namespace ide {
namespace index {

ColorScheme::ColorScheme(QColor default_fg, QColor default_bg) {
  scheme_[DEFAULT].fg = default_fg;
  scheme_[DEFAULT].bg = default_bg;
}

ColorScheme::Color& ColorScheme::operator[](Kind kind) {
  if (scheme_.contains(kind)) {
    return scheme_[kind];
  }

  return scheme_[kind] = scheme_[DEFAULT];
}

}  // namespace index
}  // namespace ide
