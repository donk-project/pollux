// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_UI_MANAGER_UI_MANAGER_H__
#define __POLLUX_POLLUX_UI_MANAGER_UI_MANAGER_H__

#include <memory>
#include <string>

#include "pollux/common/event.h"
#include "pollux/geom/vector.h"

namespace pollux {
namespace ui {

class UIState;

class UIManager {
 public:
  virtual void Setup(std::shared_ptr<ui::UIState> state) = 0;
  virtual void Draw() = 0;
  virtual bool Poll(pollux::Event& event) = 0;
  virtual void HandleEvent(pollux::Event& event) = 0;
  virtual void ShutdownAll() = 0;

  // TODO: Support auxilliary windows
};

std::shared_ptr<UIManager> MakeUIManager(
    std::shared_ptr<core::CoreManager> core_manager);

}  // namespace ui
}  // namespace pollux

#endif // __POLLUX_POLLUX_UI_MANAGER_UI_MANAGER_H__
