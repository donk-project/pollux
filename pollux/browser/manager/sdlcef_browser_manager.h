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

#ifndef __POLLUX_POLLUX_BROWSER_MANAGER_SDLCEF_BROWSER_MANAGER_H__
#define __POLLUX_POLLUX_BROWSER_MANAGER_SDLCEF_BROWSER_MANAGER_H__

#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"
#include "pollux/browser/manager/browser_manager.h"
#include "pollux/common/event.h"
#include "sdl-cef/sdl_cef_app.h"
#include "sdl-cef/sdl_cef_browser_client.h"
#include "sdl-cef/sdl_cef_events.h"
#include "sdl-cef/sdl_cef_render_handler.h"
#include "spdlog/spdlog.h"

namespace pollux {
namespace browser {

class SdlCefBrowserManager : public BrowserManager {
 public:
  void Configure() override;
  bool Initialize() override;
  void HandleEvent(pollux::Event&) override;
  void Work() override;
  void Render() override;
  void Cleanup() override;
  void ExecuteJavascript(std::string s) override;
  void SetOnScreenPosition(geom::Rect rect) override;
  bool CanRender() override;
  void EnableHighDPI(float scaling_factor) override;

  bool CanAcceptEvents() override { return cef_initialized_; }

  void ExecuteProcess(int argc, char* argv[]) {
    args_ = new CefMainArgs(argc, argv);
    auto exitCode = CefExecuteProcess(*args_, cef_app_, nullptr);
    if (exitCode >= 0) {
      spdlog::critical("CefExecuteProcess failed: {}", exitCode);
    }
  }

  void CreateRenderer(SDL_Renderer* renderer) {
    // Create the renderer handler - this takes the image buffer that CEF fills
    // using the HTML and puts it in an SDL texture
    render_handler_ = new SdlCefRenderHandler(renderer, 500, 500);
  }

  void CreateBrowser() {
    // Create a browser client - this ties into the life cycle of the browser
    browser_client_ =
        new SdlCefBrowserClient(render_handler_, *message_router_config_);

    // some browser settings
    window_info_ = new CefWindowInfo;
    window_info_->SetAsWindowless(kNullWindowHandle);

    browser_settings_ = new CefBrowserSettings;
    browser_settings_->windowless_frame_rate = 60;
    browser_settings_->background_color = 0x55ffffff;

    // Create the browser object to interpret the HTML
    std::string htmlFile = "about:blank";
    browser_ = CefBrowserHost::CreateBrowserSync(*window_info_, browser_client_,
                                                 htmlFile, *browser_settings_,
                                                 nullptr, nullptr);

    cef_initialized_ = true;
  }

 private:
  CefMessageRouterConfig* message_router_config_;
  CefMainArgs* args_;
  CefSettings* settings_;
  CefRefPtr<SdlCefApp> cef_app_;
  CefRefPtr<SdlCefRenderHandler> render_handler_;
  CefRefPtr<SdlCefBrowserClient> browser_client_;
  CefWindowInfo* window_info_;
  CefBrowserSettings* browser_settings_;
  CefRefPtr<CefBrowser> browser_;

  bool cef_initialized_ = false;
  bool cef_can_render_ = false;
};

}  // namespace browser
}  // namespace pollux

#endif  // __POLLUX_POLLUX_BROWSER_MANAGER_SDLCEF_BROWSER_MANAGER_H__
