// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_CLIENT_STATES_CLIENT_ROOM_H__
#define __POLLUX_POLLUX_CLIENT_STATES_CLIENT_ROOM_H__

#include <cstring>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "donk/api/root.h"
#include "donk/core/iota.h"
#include "donk/core/path.h"
#include "donk/interpreter/interpreter.h"
#include "pollux/core/manager/core_manager.h"
#include "pollux/networking/client/client.h"
#include "pollux/networking/constants.h"
#include "pollux/networking/net.pb.h"
#include "pollux/resources/manager/resource_manager.h"
#include "pollux/state/constants.h"
#include "pollux/state/manager.h"
#include "pollux/state/state.h"
#include "type_registrar.h"

namespace pollux {
namespace client {
namespace states {

class ClientRoom : public pollux::state::State {
 public:
  ClientRoom(std::shared_ptr<core::CoreManager> core_manager)
      : State(),
        core_manager_(core_manager),
        current_phase_(pollux::state::GameStatePhase::kUninitialized) {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::stringstream buffer;
    buffer << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    time_as_string_ = buffer.str();
  }

  ~ClientRoom() {}

  void Run() override;
  void Init();
  void HandleInput(float delta);
  void Update(float dt) override;
  pollux::state::GameStatePhase GetPhase() override { return current_phase_; }
  void GetNotifications();
  std::string GetWindowName() override;
  void Draw() override;
  void OnLogin();
  void OnBroadcast(std::string s);
  void OnUpdateFrame(networking::proto::PLXMessage &message);
  void AttemptRender(donk::var_table_t &var_table);

 private:
  void StartInterpreter() {
    auto environment = core_manager_->GetResourceManager()->GetEnvironment();
    auto collector = std::make_shared<std::map<
        donk::path_t, std::vector<std::function<void(donk::iota_t &)>>>>();
    dtpo::RegisterAll(collector);
    (*collector)[donk::path_t("/")].push_back(donk::api::Register);
    interpreter_ = donk::internal::Interpreter::Create();
    interpreter_->SetEnvironment(environment);
    interpreter_->SetRegistrationFunctions(collector);
    interpreter_->RegisterCoreprocs();
    interpreter_->CreateWorld();
  }

  bool AttemptConnection(pollux::networking::connection_key);
  void sendPlayerTextInput(std::string input);

  std::shared_ptr<core::CoreManager> core_manager_;
  pollux::state::GameStatePhase current_phase_;
  std::shared_ptr<donk::internal::Interpreter> interpreter_;
  std::shared_ptr<pollux::networking::Client> client_;
  std::string host_ip_;

  std::string time_as_string_;
  bool sort_required_ = false;
};

}  // namespace states
}  // namespace client
}  // namespace pollux

#endif  // __POLLUX_POLLUX_CLIENT_STATES_CLIENT_ROOM_H__
