// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_COMMON_EVENT_H__
#define __POLLUX_POLLUX_COMMON_EVENT_H__

#include "SDL.h"

namespace pollux {

namespace backends {
  class GworkSDL2OpenGLBackend;
}

namespace browser {
  class SdlCefBrowserManager;
}

namespace core {
  class DefaultCoreManager;
}

struct SizeEvent {
  unsigned int width;
  unsigned int height;
};

class Event {
 public:
  Event() {}
  SizeEvent size;
  bool CloseEvent() { return close_event_; }
  bool ResizeEvent() { return false; }
  bool Poll() { return true; }
  
 private:
  bool close_event_;
  SDL_Event sdl_event_;

  friend class ::pollux::backends::GworkSDL2OpenGLBackend;
  friend class ::pollux::core::DefaultCoreManager;
  friend class ::pollux::browser::SdlCefBrowserManager;
};

}  // namespace pollux

#endif  // __POLLUX_POLLUX_COMMON_EVENT_H__
