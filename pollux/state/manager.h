// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_STATE_MANAGER_H__
#define __POLLUX_POLLUX_STATE_MANAGER_H__

#include "pollux/state/constants.h"
#include "pollux/state/state.h"
#include "spdlog/spdlog.h"

namespace pollux {
namespace state {

class StateManager {
 public:
  StateManager() {}
  void activate(State *state);

  GameStatePhase CurrentPhase() {
    if (current_state_ == nullptr) {
      return GameStatePhase::kUnset;
    }
    return current_state_->GetPhase();
  }

  void Update(float dt) {
    if (CurrentPhase() == GameStatePhase::kHealthy) {
      current_state_->Update(dt);
    }
  }

  void Draw() { current_state_->Draw(); }

 private:
  State *current_state_;
};

}  // namespace state
}  // namespace pollux

#endif  // __POLLUX_POLLUX_STATE_MANAGER_H__
