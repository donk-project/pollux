// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __SNOWFROST_POLLUX_SERVER_STATES_SERVER_ROOM_H__
#define __SNOWFROST_POLLUX_SERVER_STATES_SERVER_ROOM_H__

#include <ctime>
#include <fstream>
#include <iomanip>
#include <memory>

#include "donk/interpreter/interpreter.h"
#include "enet-plus/peer.h"
#include "pollux/core/manager/core_manager.h"
#include "pollux/networking/constants.h"
#include "pollux/networking/net.pb.h"
#include "pollux/networking/server/server.h"
#include "pollux/state/constants.h"
#include "pollux/state/state.h"
#include "type_registrar.h"

namespace pollux {
namespace server {
namespace states {

class ServerRoom : public state::State {
 public:
  ServerRoom(std::shared_ptr<core::CoreManager> core_manager)
      : State(), core_manager_(core_manager) {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::stringstream buffer;
    buffer << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    time_as_string_ = buffer.str();
  }
  ~ServerRoom() {}

  void Run() override;
  void Update(float dt) override;
  pollux::state::GameStatePhase GetPhase() override { return current_phase_; }
  void ConnectClient(enet::Peer*, networking::connection_key&);
  void ClientReceive(enet::Peer*, networking::connection_key&,
                     networking::proto::PLXMessage&);
  void ClientSend(enet::Peer*, networking::connection_key&,
                  networking::proto::PLXMessage&);

  void Init();
  std::string GetWindowName() override;
  void StartInterpreter();

  virtual void HandleInput(float delta);
  void Draw() override {}

 private:
  std::shared_ptr<core::CoreManager> core_manager_;
  std::string time_as_string_;
  std::shared_ptr<donk::internal::Interpreter> interpreter_;
  std::shared_ptr<pollux::networking::Server> server_;
  pollux::state::GameStatePhase current_phase_;
};

}  // namespace states
}  // namespace server
}  // namespace pollux

#endif  // __SNOWFROST_POLLUX_SERVER_STATES_SERVER_ROOM_H__
