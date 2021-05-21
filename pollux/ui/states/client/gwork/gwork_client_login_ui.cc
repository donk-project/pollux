// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#include "pollux/ui/states/client/gwork/gwork_client_login_ui.h"

#include <cstdlib>
#include <memory>

#include "Gwork/Controls/Canvas.h"
#include "Gwork/Controls/Label.h"
#include "Gwork/Controls/TextBox.h"
#include "pollux/backends/gwork.h"
#include "pollux/core/manager/core_manager.h"
#include "pollux/ui/manager/ui_manager.h"
#include "pollux/ui/states/client/client_login_ui.h"

namespace pollux {
namespace ui {

void GworkClientLoginUI::OnConnectButtonPress(Gwk::Event::Info) {
  if (on_attempt_connection_ != nullptr) {
    ip_address_ = ip_address_textbox_->GetText();
    username_ = username_textbox_->GetText();
    on_attempt_connection_();
  }
}

void GworkClientLoginUI::OnSetup() {
  int user_id = 1000000 + std::rand() % 8999999;
  username_ = fmt::format("user{}", user_id);
  ip_address_ = "127.0.0.1";

  // TODO: Ugly cast
  auto gwork_canvas = std::dynamic_pointer_cast<backends::GworkBackend>(
                          core_manager_->GetUIManager())
                          ->GetGworkSDL2Canvas();

  {
    auto label = new Gwk::Controls::Label(gwork_canvas.get());
    label->SetPos(10, 10);
    label->SetSize(200, 40);
    label->SetText("Username");

    username_textbox_ = new Gwk::Controls::TextBox(gwork_canvas.get());
    username_textbox_->SetPos(10, 50);
    username_textbox_->SetSize(200, 40);
    username_textbox_->SetText(username_);
  }

  {
    auto label = new Gwk::Controls::Label(gwork_canvas.get());
    label->SetPos(10, 90);
    label->SetSize(200, 40);
    label->SetText("IP Address");

    ip_address_textbox_ = new Gwk::Controls::TextBox(gwork_canvas.get());
    ip_address_textbox_->SetPos(10, 130);
    ip_address_textbox_->SetSize(200, 40);
    ip_address_textbox_->SetText(ip_address_);
  }

  auto connect = new Gwk::Controls::Button(gwork_canvas.get());
  connect->SetText("Connect");
  connect->SetSize(200, 40);
  connect->SetPos(10, 180);
  connect->onPress.Add(this, &GworkClientLoginUI::OnConnectButtonPress);

  // auto& sdlgui_screen = std::dynamic_pointer_cast<backends::GworkBackend>(
  //                           core_manager_->GetUIManager())
  //                           ->GetGworkScreen();
  // auto& nwindow = sdlgui_screen.window("Donk Client", sdlgui::Vector2i{30,
  // 30}); nwindow.withLayout<sdlgui::GroupLayout>();

  // auto& ip_address = nwindow.wdg<sdlgui::TextBox>();
  // ip_address.setAlignment(sdlgui::TextBox::Alignment::Left);
  // ip_address.setEditable(true);
  // ip_address.setValue(ip_address_);
  // ip_address.setCallback([=](const std::string& s) {
  //   ip_address_ = s;
  //   return true;
  // });

  // auto& username = nwindow.wdg<sdlgui::TextBox>();
  // username.setAlignment(sdlgui::TextBox::Alignment::Left);
  // username.setEditable(true);
  // username.setValue(username_);
  // username.setCallback([=](const std::string& s) {
  //   username_ = s;
  //   return true;
  // });

  // nwindow.button("Connect", [=, &nwindow] {
  //   if (on_attempt_connection_ != nullptr) {
  //     auto r = on_attempt_connection_();
  //     if (r) {
  //       nwindow.dispose();
  //     }
  //   }
  // });
}

void GworkClientLoginUI::OnDraw() {}

void GworkClientLoginUI::OnTeardown() {}

void GworkClientLoginUI::OnResize(int w, int h) {}

void GworkClientLoginUI::OnAttemptConnection(std::function<bool()> f) {
  on_attempt_connection_ = f;
}

std::string GworkClientLoginUI::GetIpAddress() { return ip_address_; }

std::string GworkClientLoginUI::GetUsername() { return username_; }

std::shared_ptr<ClientLoginUI> MakeClientLoginUI(
    std::shared_ptr<core::CoreManager> core_manager) {
  auto ui = std::make_shared<GworkClientLoginUI>(core_manager);
  return ui;
}

}  // namespace ui
}  // namespace pollux
