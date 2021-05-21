// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_CORE_MANAGER_CORE_MANAGER_H__
#define __POLLUX_POLLUX_CORE_MANAGER_CORE_MANAGER_H__

#include <stdlib.h>

#include <exception>
#include <memory>
#include <string>

#include "pollux/geom/vector.h"
#include "pollux/render/manager/render_manager.h"
#include "pollux/browser/manager/browser_manager.h"
#include "pollux/resources/manager/resource_manager.h"
#include "pollux/state/manager.h"
#include "pollux/ui/manager/ui_manager.h"
#include "pollux/ui/manager/ui_state.h"

namespace pollux {
namespace core {

class CoreManager {
 public:
  virtual ~CoreManager() {}

  virtual void ActivateState(state::State* state) = 0;
  virtual std::shared_ptr<ui::UIManager> GetUIManager() = 0;
  virtual std::shared_ptr<render::RenderManager> GetRenderManager() = 0;
  virtual std::shared_ptr<resources::ResourceManager> GetResourceManager() = 0;
  virtual std::shared_ptr<state::StateManager> GetStateManager() = 0;
  virtual std::shared_ptr<browser::BrowserManager> GetBrowserManager() = 0;
  virtual void StartGameLoop() = 0;
};

std::shared_ptr<CoreManager> MakeCoreManager();

}  // namespace core
}  // namespace pollux

#endif // __POLLUX_POLLUX_CORE_MANAGER_CORE_MANAGER_H__
