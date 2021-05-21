// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __SNOWFROST_POLLUX_NETWORKING_CONSTANTS_H__
#define __SNOWFROST_POLLUX_NETWORKING_CONSTANTS_H__

namespace pollux {
namespace networking {

enum class PacketPrefix {
  kLoginSuccess = 100,
  kLoginIncomplete,
  kLoginFailure,
  kServerReceiveInput,
  kClientConnectUpdate,
  kClientReceiveBroadcast,
};

enum class ClientMode {
  kInitializing = 200,
  kReadyToConnect,
  kWaiting,
  kReceivingUpdateFrame,
  kReceivedUpdateFrame,
  kLoginAccepted,
  kReceivingBroadcast,
  kReceivedBroadcast,
};

enum class ServerMode { kInitializing = 300, kReady };

enum class ClientState {
  kNew = 400,
  kAuthenticated,
};

struct connection_key {
  std::string ip;
  uint16_t port;
  std::string username;
  std::string password_hash_;
  ClientState client_state;
};

}  // namespace networking
}  // namespace pollux

#endif  // __SNOWFROST_POLLUX_NETWORKING_CONSTANTS_H__
