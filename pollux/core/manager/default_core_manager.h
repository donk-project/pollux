// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_CORE_MANAGER_DEFAULT_CORE_MANAGER_H__
#define __POLLUX_POLLUX_CORE_MANAGER_DEFAULT_CORE_MANAGER_H__

#include <stdlib.h>

#include <exception>
#include <memory>
#include <string>

#include "pollux/core/manager/core_manager.h"
#include "pollux/geom/vector.h"
#include "pollux/state/manager.h"
#include "pollux/state/state.h"
#include "pollux/browser/manager/browser_manager.h"
#include "pollux/ui/manager/ui_manager.h"
#include "spdlog/spdlog.h"

namespace pollux {
namespace core {

class DefaultCoreManager
    : public std::enable_shared_from_this<DefaultCoreManager>,
      public CoreManager {
 public:
  template <typename... T>
  static std::shared_ptr<DefaultCoreManager> Create(T&&... t) {
    return std::shared_ptr<DefaultCoreManager>(
        new DefaultCoreManager(std::forward<T>(t)...));
  }

  std::shared_ptr<ui::UIManager> GetUIManager() override { return ui_manager_; }

  std::shared_ptr<render::RenderManager> GetRenderManager() override {
    return render_manager_;
  }

  std::shared_ptr<resources::ResourceManager> GetResourceManager() override {
    return resource_manager_;
  }

  std::shared_ptr<state::StateManager> GetStateManager() override {
    return state_manager_;
  }

  std::shared_ptr<browser::BrowserManager> GetBrowserManager() override {
    return browser_manager_;
  }

  void SetRenderManager(std::shared_ptr<render::RenderManager> rm) {
    render_manager_ = rm;
  }

  void SetResourceManager(std::shared_ptr<resources::ResourceManager> rm) {
    resource_manager_ = rm;
  }

  void SetStateManager(std::shared_ptr<state::StateManager> sm) {
    state_manager_ = sm;
  }

  void SetUIManager(std::shared_ptr<ui::UIManager> uim) { ui_manager_ = uim; }

  void SetBrowserManager(std::shared_ptr<browser::BrowserManager> bm) { browser_manager_ = bm; }

  void ActivateState(pollux::state::State* state) override;
  void StartGameLoop() override;
  
 private:
  DefaultCoreManager();
  std::shared_ptr<resources::ResourceManager> resource_manager_;
  std::shared_ptr<ui::UIManager> ui_manager_;
  std::shared_ptr<state::StateManager> state_manager_;
  std::shared_ptr<render::RenderManager> render_manager_;
  std::shared_ptr<browser::BrowserManager> browser_manager_;
};

std::shared_ptr<CoreManager> MakeCoreManager();

}  // namespace core
}  // namespace pollux

#endif // __POLLUX_POLLUX_CORE_MANAGER_DEFAULT_CORE_MANAGER_H__
