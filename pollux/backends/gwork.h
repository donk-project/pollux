// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_BACKENDS_GWORK_H__
#define __POLLUX_POLLUX_BACKENDS_GWORK_H__

#include "Gwork/Renderers/SDL2.h"

namespace pollux {
namespace backends {

class GworkBackend {
 public:
  virtual std::shared_ptr<Gwk::Renderer::SDL2> GetGworkSDL2Renderer() = 0;
  virtual std::shared_ptr<Gwk::Controls::Canvas> GetGworkSDL2Canvas() = 0;
};

}  // namespace backends
}  // namespace pollux

#endif // __POLLUX_POLLUX_BACKENDS_GWORK_H__
