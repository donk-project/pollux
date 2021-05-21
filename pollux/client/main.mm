// This file combines implementations from cef-project's
// examples/shared/main_mac.mm And sdl2-cef's sdl_cef_main.cc in order to get
// the correct Cocoa environment established while also initializing the SDL
// environment alongside the Chromium processes.

// Portions copyright (c) 2017 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2010 The Chromium Authors. All rights reserved.
// Portions copyright (c) 2008-2016 Marshall A. Greenblatt.
// Portions copyright (c) 2006-2009 Google Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#import <Cocoa/Cocoa.h>

#import "examples/shared/shared_app_delegate.h"
#import "examples/shared/shared_application.h"
#import "include/wrapper/cef_library_loader.h"

#include <iostream>

#include "SDL.h"
#include "SDL_image.h"
#include "examples/shared/app_factory.h"
#include "examples/shared/client_manager.h"
#include "include/cef_app.h"
#include "include/cef_application_mac.h"
#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"
#include "sdl-cef/sdl_cef_app.h"
#include "sdl-cef/sdl_cef_browser_client.h"
#include "sdl-cef/sdl_cef_events.h"
#include "sdl-cef/sdl_cef_render_handler.h"
#include "spdlog/spdlog.h"

#include "pollux/client/states/client_room.h"
#include "pollux/core/manager/core_manager.h"
#include "pollux/core/manager/default_core_manager.h"
#include "pollux/browser/manager/sdlcef_browser_manager.h"
#include "pollux/resources/manager/mac_resource_manager.h" 
#include "pollux/backends/gwork_sdl2/backend.h"

int main(int argc, char* argv[]) {
  // Load the CEF framework library at runtime instead of linking directly
  // as required by the macOS sandbox implementation.
  CefScopedLibraryLoader library_loader;
  if (!library_loader.LoadInMain()) {
    spdlog::critical("Cannot load CEF library loader");
    return 1;
  }

  auto sdlcef_browser_manager = std::make_shared<pollux::browser::SdlCefBrowserManager>();
  sdlcef_browser_manager->Configure();
  sdlcef_browser_manager->ExecuteProcess(argc, argv);

  // Initialize the AutoRelease pool.
  NSAutoreleasePool* autopool = [[NSAutoreleasePool alloc] init];

  // Initialize the SharedApplication instance.
  [SharedApplication sharedApplication];

  // Create the singleton manager instance.
  shared::ClientManager manager;

  auto core_manager = pollux::core::DefaultCoreManager::Create();

  auto resource_manager =
      std::make_shared<pollux::resources::MacResourceManager>(core_manager, argv[0]);
  auto backend =
      std::make_shared<pollux::backends::GworkSDL2OpenGLBackend>(core_manager);
  auto state_manager = std::make_shared<pollux::state::StateManager>();

  core_manager->SetUIManager(backend);
  core_manager->SetRenderManager(backend);
  core_manager->SetResourceManager(resource_manager);
  core_manager->SetStateManager(state_manager);
  core_manager->SetBrowserManager(sdlcef_browser_manager);
  core_manager->ActivateState(
      new pollux::client::states::ClientRoom(core_manager));

  if (!sdlcef_browser_manager->Initialize()) {
    spdlog::critical("Cannot initialize CEF.");
    return 1;
  }

  NSObject* delegate = [[SharedAppDelegate alloc] init];
  [delegate performSelectorOnMainThread:@selector(createApplication:)
                             withObject:nil
                          waitUntilDone:NO];

  sdlcef_browser_manager->CreateRenderer(backend->GetSDL2Renderer());
  sdlcef_browser_manager->CreateBrowser();

  core_manager->StartGameLoop();

  sdlcef_browser_manager->Cleanup();

  // Release the delegate.
  [delegate release];

  // Release the AutoRelease pool.
  [autopool release];

  return 0;
}
