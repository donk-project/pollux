// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_GEOM_VECTOR_H__
#define __POLLUX_POLLUX_GEOM_VECTOR_H__

#include "SDL2pp/Point.hh"

namespace pollux {
namespace geom {

class Vector2u {
 public:
  Vector2u(int x, int y) : point_(x, y) {}

  void x(int i) { point_.x = i; }
  void y(int i) { point_.y = i; }
  int x() { return point_.x; }
  int y() { return point_.y; }

 private:
  SDL2pp::Point point_;
};

class Vector2f {
 public:
  Vector2f(float x, float y) : x_(x), y_(y) {}

 private:
  float x_;
  float y_;
};

}  // namespace geom
}  // namespace pollux

#endif // __POLLUX_POLLUX_GEOM_VECTOR_H__
