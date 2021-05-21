// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_STATE_STATE_H__
#define __POLLUX_POLLUX_STATE_STATE_H__

#include <memory>
#include <string>

#include "pollux/state/constants.h"

namespace pollux {
namespace state {

class State {
 public:
  virtual void Run() = 0;
  virtual void Update(float dt) = 0;
  virtual void Draw() = 0;
  virtual std::string GetWindowName() = 0;
  virtual GameStatePhase GetPhase() = 0;
  virtual ~State() {}
};

}  // namespace state
}  // namespace pollux

#endif // __POLLUX_POLLUX_STATE_STATE_H__
