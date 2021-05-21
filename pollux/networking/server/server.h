// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __SNOWFROST_POLLUX_NETWORKING_SERVER_SERVER_H__
#define __SNOWFROST_POLLUX_NETWORKING_SERVER_SERVER_H__

#include <functional>
#include <map>

#include "enet-plus/enet.h"
#include "librg/librg.h"
#include "pollux/networking/constants.h"
#include "pollux/networking/net.pb.h"

namespace enet {
class Event;
class ServerHost;
}  // namespace enet

namespace pollux::networking {

class Server {
 public:
  bool Initialize();
  void Update(float delta);

  void DeliverMessage(proto::PLXMessage&, enet::Peer*);

  void OnConnectedClient(std::function<void(enet::Peer*, connection_key&)> f) {
    on_connected_client_ = f;
  }

  void OnClientReceive(
      std::function<void(enet::Peer*, connection_key&, proto::PLXMessage&)> f) {
    on_client_receive_ = f;
  }

  void OnClientSend(
      std::function<void(enet::Peer*, connection_key&, proto::PLXMessage&)> f) {
    on_client_send_ = f;
  }

  std::map<enet::Peer*, std::shared_ptr<connection_key>>& clients() {
    return clients_;
  }

 private:
  std::function<void(enet::Peer*, connection_key&)> on_connected_client_;
  std::function<void(enet::Peer*, connection_key&, proto::PLXMessage&)>
      on_client_receive_;
  std::function<void(enet::Peer*, connection_key&, proto::PLXMessage&)>
      on_client_send_;

  std::map<enet::Peer*, std::shared_ptr<connection_key>> clients_;

  enet::Enet* enet_;
  enet::ServerHost* server_host_;
  librg_world* tracking_world_;
};

}  // namespace pollux::networking

#endif  // __SNOWFROST_POLLUX_NETWORKING_SERVER_SERVER_H__
