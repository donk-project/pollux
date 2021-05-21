// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __SNOWFROST_POLLUX_NETWORKING_CLIENT_CLIENT_H__
#define __SNOWFROST_POLLUX_NETWORKING_CLIENT_CLIENT_H__

#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "enet-plus/enet.h"
#include "pollux/networking/constants.h"
#include "pollux/networking/net.pb.h"

namespace enet {
class ClientHost;
class Event;
class Peer;
}  // namespace enet

namespace pollux {
namespace networking {

class Client {
 public:
  Client(connection_key nc)
      : connection_key_(nc),
        mode_(pollux::networking::ClientMode::kInitializing) {}

  bool Connect();
  void Disconnect();
  void GetNotifications();

  void OnLogin(std::function<void()>);
  void OnBroadcast(std::function<void(std::string)>);
  void OnUpdateFrame(std::function<void(proto::PLXMessage&)>);

  void SendUserCommand(std::string);
  bool is_connected() const { return is_connected_; }

 private:
  std::function<void()> on_login_;
  std::function<void(std::string)> on_broadcast_;
  std::function<void(proto::PLXMessage&)> on_update_frame_;

  connection_key connection_key_;
  bool is_connected_ = false;

  pollux::networking::ClientMode mode_;
  enet::Enet* enet_;
  enet::ClientHost* client_;
  enet::Peer* peer_;
};

}  // namespace networking
}  // namespace pollux

#endif  // __SNOWFROST_POLLUX_NETWORKING_CLIENT_CLIENT_H__
