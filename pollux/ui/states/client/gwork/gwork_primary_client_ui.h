// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_UI_STATES_CLIENT_GWORK_GWORK_PRIMARY_CLIENT_UI_H__
#define __POLLUX_POLLUX_UI_STATES_CLIENT_GWORK_GWORK_PRIMARY_CLIENT_UI_H__

#include <functional>
#include <memory>

#include "Gwork/Controls/Base.h"
#include "Gwork/Controls/TextBox.h"
#include "Gwork/Controls/TabControl.h"
#include "Gwork/Controls/WindowControl.h"
#include "Gwork/Events.h"
#include "pollux/core/manager/core_manager.h"
#include "pollux/ui/elements/viewport_2d.h"
#include "pollux/ui/manager/ui_state.h"
#include "pollux/ui/states/client/primary_client_ui.h"

namespace pollux {
namespace ui {

class SidePanel : public Gwk::Controls::WindowControl {
 public:
  SidePanel(Gwk::Controls::Base* parent) : WindowControl(parent) {
    // Disable dragging window via the title bar
    m_titleBar->SetDisabled(true);
  }
};

class GworkPrimaryClientUI : public PrimaryClientUI,
                             public Gwk::Event::Handler {
 public:
  GworkPrimaryClientUI(std::shared_ptr<core::CoreManager> core_manager)
      : PrimaryClientUI(), core_manager_(core_manager) {}

  void OnSetup() override;
  void OnDraw() override;
  void OnTeardown() override;
  void OnResize(int w, int h) override;
  geom::Rect GetSuggestedBrowserRect() override;

  void OnChatboxEnter(std::function<void(const std::string&)> f) {
    on_chatbox_enter_ = f;
  }

 private:
  void OnSubmitCommand(Gwk::Event::Info);

  std::function<void(const std::string&)> on_chatbox_enter_;
  std::unique_ptr<ui::Viewport2D> viewport2d_;
  std::shared_ptr<core::CoreManager> core_manager_;
  std::shared_ptr<Gwk::Controls::TextBox> command_input_;
  std::shared_ptr<SidePanel> side_panel_window_;
  std::shared_ptr<Gwk::Controls::TabControl> statpanels_;
  std::shared_ptr<Gwk::Controls::TextBox> world_output_;


  int command_input_height = 40;
  int side_panel_width = 400;
};

}  // namespace ui
}  // namespace pollux

#endif  // __POLLUX_POLLUX_UI_STATES_CLIENT_GWORK_GWORK_PRIMARY_CLIENT_UI_H__
