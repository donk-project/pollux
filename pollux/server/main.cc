// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#include "pollux/backends/gwork_sdl2/backend.h"
#include "pollux/core/manager/core_manager.h"
#include "pollux/core/manager/default_core_manager.h"
#include "pollux/resources/manager/default_resource_manager.h"
#include "pollux/server/states/server_room.h"

// TODO: Fuck SDL and get SDL_main.h out of SDL.h
#undef main

int main(int argc, char** argv) {
  spdlog::info("Donk Server.");
  auto core_manager = pollux::core::DefaultCoreManager::Create();

  auto resource_manager =
      std::make_shared<pollux::resources::DefaultResourceManager>(core_manager,
                                                                  argv[0]);
  auto backend =
      std::make_shared<pollux::backends::GworkSDL2OpenGLBackend>(core_manager);
  auto state_manager = std::make_shared<pollux::state::StateManager>();

  core_manager->SetUIManager(backend);
  core_manager->SetRenderManager(backend);
  core_manager->SetResourceManager(resource_manager);
  core_manager->SetStateManager(state_manager);
  core_manager->ActivateState(
      new pollux::server::states::ServerRoom(core_manager));
  core_manager->StartGameLoop();

  return 0;
}
