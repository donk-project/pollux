// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_UI_STATES_CLIENT_PRIMARY_CLIENT_UI_H__
#define __POLLUX_POLLUX_UI_STATES_CLIENT_PRIMARY_CLIENT_UI_H__

#include <functional>
#include <memory>

#include "pollux/core/manager/core_manager.h"
#include "pollux/ui/manager/ui_state.h"

namespace pollux {
namespace ui {

class PrimaryClientUI : public UIState {
 public:
  virtual void OnChatboxEnter(std::function<void(const std::string&)> f) = 0;
  virtual geom::Rect GetSuggestedBrowserRect() = 0;
};

std::shared_ptr<PrimaryClientUI> MakePrimaryClientUI(std::shared_ptr<core::CoreManager> core_manager);

}  // namespace ui
}  // namespace pollux

#endif // __POLLUX_POLLUX_UI_STATES_CLIENT_PRIMARY_CLIENT_UI_H__
