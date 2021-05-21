// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_RENDER_TEXTURE_H__
#define __POLLUX_POLLUX_RENDER_TEXTURE_H__

#include "SDL2pp/Texture.hh"

namespace pollux {
namespace render {

class Texture {
 public:
 private:
  std::unique_ptr<SDL2pp::Texture> texture_;
};

}  // namespace render
}  // namespace pollux

#endif  // __POLLUX_POLLUX_RENDER_TEXTURE_H__
