// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#include "pollux/ui/states/client/gwork/gwork_primary_client_ui.h"

#include "Gwork/Controls/Canvas.h"
#include "Gwork/Controls/TextBox.h"
#include "Gwork/Events.h"
#include "pollux/backends/gwork.h"
#include "pollux/ui/manager/ui_manager.h"
#include "pollux/ui/states/client/primary_client_ui.h"

namespace pollux {
namespace ui {

void GworkPrimaryClientUI::OnSetup() {
  // TODO: Ugly cast
  auto gwork_canvas = std::dynamic_pointer_cast<backends::GworkBackend>(
                          core_manager_->GetUIManager())
                          ->GetGworkSDL2Canvas();

  gwork_canvas->RemoveAllChildren();
  gwork_canvas->SetDrawBackground(false);

  command_input_ = std::make_shared<Gwk::Controls::TextBox>(gwork_canvas.get());
  command_input_->SetPos(0, gwork_canvas->Height() - command_input_height);
  command_input_->SetSize(gwork_canvas->Width(), command_input_height);
  command_input_->onReturnPressed.Add(this,
                                      &GworkPrimaryClientUI::OnSubmitCommand);
  command_input_->Focus();

  side_panel_window_ = std::make_shared<SidePanel>(gwork_canvas.get());
  side_panel_window_->SetPos(gwork_canvas->Width() - side_panel_width, 0);
  side_panel_window_->SetSize(side_panel_width,
                              gwork_canvas->Height() - command_input_height);
  side_panel_window_->SetClosable(false);
  side_panel_window_->SetTitle("");

  // Disable all resizers but the left-side (index 6)
  side_panel_window_->GetResizer(0)->Hide();
  side_panel_window_->GetResizer(1)->Hide();
  side_panel_window_->GetResizer(2)->Hide();
  side_panel_window_->GetResizer(3)->Hide();
  side_panel_window_->GetResizer(4)->Hide();
  side_panel_window_->GetResizer(5)->Hide();
  side_panel_window_->GetResizer(7)->Hide();
}

void GworkPrimaryClientUI::OnResize(int w, int h) {
  // TODO: Ugly cast
  auto gwork_canvas = std::dynamic_pointer_cast<backends::GworkBackend>(
                          core_manager_->GetUIManager())
                          ->GetGworkSDL2Canvas();

  command_input_->SetPos(0, gwork_canvas->Height() - command_input_height);
  command_input_->SetSize(gwork_canvas->Width(), command_input_height);
  command_input_->Invalidate();
  side_panel_window_->SetWidth(side_panel_width);
  side_panel_window_->SetPos(gwork_canvas->Width() - side_panel_width, 0);
  side_panel_window_->SetHeight(gwork_canvas->Height() - command_input_height);
  side_panel_window_->Invalidate();
}

geom::Rect GworkPrimaryClientUI::GetSuggestedBrowserRect() {
  auto gwork_canvas = std::dynamic_pointer_cast<backends::GworkBackend>(
                          core_manager_->GetUIManager())
                          ->GetGworkSDL2Canvas();
  geom::Rect rect;
  rect.x((side_panel_window_->X() + 8 /* left window border */) * gwork_canvas->Scale());
  rect.y((side_panel_window_->Y() + 28 /*title bar height plus a bit */) *
         gwork_canvas->Scale());
  rect.w(side_panel_width * gwork_canvas->Scale());
  rect.h((gwork_canvas->Height() - command_input_height - 28) *
         gwork_canvas->Scale());
  return rect;
}

void GworkPrimaryClientUI::OnSubmitCommand(Gwk::Event::Info) {
  if (on_chatbox_enter_ != nullptr) {
    on_chatbox_enter_(command_input_->GetText());
  }
  command_input_->SetText("");
  command_input_->Focus();
}

void GworkPrimaryClientUI::OnDraw() {
  // primary_canvas_->display();
}

void GworkPrimaryClientUI::OnTeardown() {
  // primary_canvas_->clear();
}

std::shared_ptr<PrimaryClientUI> MakePrimaryClientUI(
    std::shared_ptr<core::CoreManager> core_manager) {
  auto ui = std::make_shared<GworkPrimaryClientUI>(core_manager);
  return ui;
}

}  // namespace ui
}  // namespace pollux
