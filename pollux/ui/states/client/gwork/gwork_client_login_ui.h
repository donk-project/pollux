// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_UI_STATES_CLIENT_GWORK_GWORK_CLIENT_LOGIN_UI_H__
#define __POLLUX_POLLUX_UI_STATES_CLIENT_GWORK_GWORK_CLIENT_LOGIN_UI_H__

#include <functional>
#include <memory>

#include "Gwork/Events.h"
#include "Gwork/Controls/TextBox.h"
#include "pollux/core/manager/core_manager.h"
#include "pollux/ui/manager/ui_state.h"
#include "pollux/ui/states/client/client_login_ui.h"

namespace pollux {
namespace ui {

class GworkClientLoginUI : public ClientLoginUI, public Gwk::Event::Handler {
 public:
  GworkClientLoginUI(std::shared_ptr<core::CoreManager> core_manager)
      : ClientLoginUI(), core_manager_(core_manager) {}

  void OnSetup() override;
  void OnDraw() override;
  void OnTeardown() override;
  void OnResize(int w, int h) override;

  void OnAttemptConnection(std::function<bool()>) override;
  std::string GetIpAddress() override;
  std::string GetUsername() override;

 protected:
  std::function<bool()> on_attempt_connection_;
  std::string username_;
  std::string ip_address_;
  std::shared_ptr<core::CoreManager> core_manager_;

 private:
  void OnConnectButtonPress(Gwk::Event::Info);

  Gwk::Controls::TextBox* username_textbox_;  
  Gwk::Controls::TextBox* ip_address_textbox_;  
};

}  // namespace ui
}  // namespace pollux

#endif  // __POLLUX_POLLUX_UI_STATES_CLIENT_GWORK_GWORK_CLIENT_LOGIN_UI_H__
