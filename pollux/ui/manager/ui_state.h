// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_UI_MANAGER_UI_STATE_H__
#define __POLLUX_POLLUX_UI_MANAGER_UI_STATE_H__

#include <memory>

namespace pollux {
namespace ui {

class UIState {
 public:
  virtual ~UIState() {}

  virtual void OnSetup() = 0;
  virtual void OnDraw() = 0;
  virtual void OnResize(int w, int h) = 0;
  virtual void OnTeardown() = 0;
};

}  // namespace ui
}  // namespace pollux

#endif  // __POLLUX_POLLUX_UI_MANAGER_UI_STATE_H__
