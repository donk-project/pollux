// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_GEOM_RECT_H__
#define __POLLUX_POLLUX_GEOM_RECT_H__

#include "SDL2pp/Rect.hh"
#include "fmt/format.h"

namespace pollux {
namespace geom {

class Rect {
 public:
  Rect() : rect_(0, 0, 0, 0) {}
  Rect(int x, int y, int w, int h) : rect_(x, y, w, h) {}

  void x(int i) { rect_.x = i; }
  void y(int i) { rect_.y = i; }
  void w(int i) { rect_.w = i; }
  void h(int i) { rect_.h = i; }

  int x() const { return rect_.x; }
  int y() const { return rect_.y; }
  int w() const { return rect_.w; }
  int h() const { return rect_.h; }

 private:
  SDL2pp::Rect rect_;
};

}  // namespace geom
}  // namespace pollux

namespace fmt {

template <>
struct fmt::formatter<::pollux::geom::Rect> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(::pollux::geom::Rect const& r, FormatContext& ctx) {
    return fmt::format_to(ctx.out(), "Rect<{} {} {} {}>", r.x(), r.y(), r.w(), r.h());
  }
};

}  // namespace fmt

#endif // __POLLUX_POLLUX_GEOM_RECT_H__
