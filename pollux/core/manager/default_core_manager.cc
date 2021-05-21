// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#include "pollux/core/manager/default_core_manager.h"

#include <stdlib.h>

#include <exception>
#include <memory>

#include "pollux/render/manager/render_manager.h"
#include "pollux/resources/manager/resource_manager.h"
#include "pollux/state/manager.h"
#include "pollux/ui/manager/ui_manager.h"
#include "spdlog/spdlog.h"

namespace pollux {
namespace core {

DefaultCoreManager::DefaultCoreManager() {}

void DefaultCoreManager::ActivateState(pollux::state::State* state) {
  render_manager_->BuildWindow(state->GetWindowName());
  state_manager_->activate(state);
}

int draw_mode = 1;
int TICK_INTERVAL = 20;
Uint32 next_time;

Uint32 timeLeft(void) {
  Uint32 now;
  now = SDL_GetTicks();
  if (next_time <= now)
    return 0;
  else
    return next_time - now;
}

void capFramerate(void) {
  SDL_Delay(timeLeft());
  next_time += TICK_INTERVAL;
}

void DefaultCoreManager::StartGameLoop() {
  auto start = std::chrono::steady_clock::now();
  // number of the current frame
  int frameNum = SDL_GetTicks() / TICK_INTERVAL;
  // amount of rendered frames
  int renderedFrames = 0;

  // desired frames by second
  int framesBySecond = 1000 / TICK_INTERVAL;
  while (render_manager_->IsOpen()) {
    auto next = std::chrono::steady_clock::now();
    std::chrono::duration<float, std::milli> duration =
        std::chrono::steady_clock::now() - start;
    float dt = duration.count();
    pollux::Event event;

    if (SDL_GetTicks() / TICK_INTERVAL > frameNum) {
      frameNum = SDL_GetTicks() / TICK_INTERVAL;
    }

    while (SDL_PollEvent(&event.sdl_event_) != 0) {
      if (event.sdl_event_.type == SDL_QUIT) {
        ui_manager_->ShutdownAll();
        render_manager_->CloseWindow();
        return;
      }
      ui_manager_->HandleEvent(event);
      if (browser_manager_ != nullptr && browser_manager_->CanAcceptEvents()) {
        browser_manager_->HandleEvent(event);
      }
    }

    if (browser_manager_ != nullptr) {
      browser_manager_->Work();
    }
    // while (event.Poll()) {
    //   ui_manager_->HandleEvent(event);

    //   if (event.CloseEvent()) {
    //     ui_manager_->Cleanup();
    //     render_manager_->CloseWindow();
    //     return;
    //   }

    //   if (event.ResizeEvent()) {
    //     ui_manager_->HandleResize(event);
    //   }
    // }

    state_manager_->Update(dt);
    render_manager_->PreDraw();
    state_manager_->Draw();
    ui_manager_->Draw();
    if (browser_manager_ != nullptr) {
      browser_manager_->Render();
    }
    render_manager_->PostDraw();

    if (draw_mode == 1) {
      capFramerate();
    }

    renderedFrames++;
  }
}

std::shared_ptr<CoreManager> MakeCoreManager() {
  auto core_manager = DefaultCoreManager::Create();
  return core_manager;
}

}  // namespace core
}  // namespace pollux
