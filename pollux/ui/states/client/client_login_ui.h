// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_UI_STATES_CLIENT_CLIENT_LOGIN_UI_H__
#define __POLLUX_POLLUX_UI_STATES_CLIENT_CLIENT_LOGIN_UI_H__

#include <functional>
#include <memory>

#include "pollux/core/manager/core_manager.h"
#include "pollux/ui/manager/ui_state.h"

namespace pollux {
namespace ui {

class ClientLoginUI : public UIState {
 public:
  virtual void OnAttemptConnection(std::function<bool()>) = 0;
  virtual std::string GetIpAddress() = 0;
  virtual std::string GetUsername() = 0;
};

std::shared_ptr<ClientLoginUI> MakeClientLoginUI(std::shared_ptr<core::CoreManager> core_manager);

}  // namespace ui
}  // namespace pollux

#endif // __POLLUX_POLLUX_UI_STATES_CLIENT_CLIENT_LOGIN_UI_H__
