// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_UI_ELEMENTS_VIEWPORT_2D_H__
#define __POLLUX_POLLUX_UI_ELEMENTS_VIEWPORT_2D_H__

#include "pollux/resources/manager/resource_manager.h"

namespace pollux {
namespace ui {

class Viewport2D {
 public:
  Viewport2D(std::shared_ptr<resources::ResourceManager> resource_manager)
      : resource_manager_(resource_manager), current_zlevel_(1) {}

 private:
  std::shared_ptr<resources::ResourceManager> resource_manager_;
  int current_zlevel_;
};

}  // namespace ui
}  // namespace pollux

#endif // __POLLUX_POLLUX_UI_ELEMENTS_VIEWPORT_2D_H__
