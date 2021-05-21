// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#include "pollux/state/manager.h"

#include "pollux/state/state.h"

namespace pollux::state {

void StateManager::activate(State *state) {
  current_state_ = state;
  current_state_->Run();
}

}  // namespace pollux::state
