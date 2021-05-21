// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_STATE_CONSTANTS_H__
#define __POLLUX_POLLUX_STATE_CONSTANTS_H__

namespace pollux {
namespace state {

enum class GameStatePhase {
  kUnset = 100,
  kUninitialized,
  kInitializing,
  kHealthy,
};

}  // namespace state
}  // namespace pollux

#endif // __POLLUX_POLLUX_STATE_CONSTANTS_H__
