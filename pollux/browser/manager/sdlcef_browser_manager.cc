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

#include "pollux/browser/manager/sdlcef_browser_manager.h"

#include <cmath>

#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"
#include "pollux/common/event.h"
#include "pollux/geom/rect.h"

namespace pollux {
namespace browser {

void SdlCefBrowserManager::Configure() {
  message_router_config_ = new CefMessageRouterConfig;
  cef_app_ = new SdlCefApp(*message_router_config_);
}

bool SdlCefBrowserManager::Initialize() {
  // Initialize Chromium Embedded Framework
  CefSettings settings;
  settings.windowless_rendering_enabled = true;
  settings.multi_threaded_message_loop = false;
  settings.external_message_pump = true;
  if (!CefInitialize(*args_, settings, cef_app_, nullptr)) {
    return false;
  }
  return true;
}

void SdlCefBrowserManager::HandleEvent(pollux::Event& event) {
  SDL_Event& e = event.sdl_event_;
  // handle quit and window resize events separately
  if (e.type == SDL_QUIT) {
    browser_->GetHost()->CloseBrowser(false);
  } else if (e.type == SDL_WINDOWEVENT &&
             e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
    render_handler_->resize(e.window.data1, e.window.data2);
    browser_->GetHost()->WasResized();
  } else {
    handleEvent(e, browser_.get());
  }
}

void SdlCefBrowserManager::Work() {
  if (cef_app_ != nullptr) {
    cef_app_->doCefWork();
  }
}

void SdlCefBrowserManager::Render() {
  if (render_handler_ != nullptr) {
    render_handler_->render();
  }
}

void SdlCefBrowserManager::Cleanup() {
  browser_ = nullptr;
  browser_client_ = nullptr;
  render_handler_ = nullptr;
  if (cef_initialized_) {
    CefShutdown();
  }
}
bool SdlCefBrowserManager::CanRender() {
  return render_handler_ != nullptr && browser_client_ != nullptr;
}

void SdlCefBrowserManager::SetOnScreenPosition(geom::Rect rect) {
  if (render_handler_ != nullptr) {
    spdlog::info("SetOnScreenPosition: {}", rect);
    render_handler_->set_position(rect.x(), rect.y());
    render_handler_->resize(rect.w(), rect.h());
    browser_->GetHost()->WasResized();
  }
}

void SdlCefBrowserManager::ExecuteJavascript(std::string s) {
  // TODO: Use much more robust checks on browser state before doing anything
  // (e.g. DOMContentLoaded).
  if (browser_ != nullptr) {
    auto frame = browser_->GetMainFrame();
    frame->ExecuteJavaScript(s, frame->GetURL(), 0);
  }
}

void SdlCefBrowserManager::EnableHighDPI(float scaling_factor) {
  CefEnableHighDPISupport();
  if (render_handler_ != nullptr) {
    render_handler_->set_scaling_factor(scaling_factor);
    if (browser_ != nullptr) {
      // SetZoomLevel isn't percentage based, instead you have to calculate
      // the zoom level based on the renderer's internal scaling algorithm.
      // See https://www.magpcss.org/ceforum/viewtopic.php?f=6&t=11491.
      float zoom_level = std::log(scaling_factor) / std::log(1.2);
      browser_->GetHost()->SetZoomLevel(zoom_level);
      browser_->GetHost()->NotifyScreenInfoChanged();
    }
  } else {
    spdlog::critical("too early to set browser scaling factor");
  }
}

}  // namespace browser
}  // namespace pollux
