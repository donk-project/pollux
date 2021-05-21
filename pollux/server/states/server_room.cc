// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#include "pollux/server/states/server_room.h"

#include <cstdlib>
#include <ctime>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "donk/api/client.h"
#include "donk/core/iota.h"
#include "donk/core/path.h"
#include "donk/interpreter/ecs_manager.h"
#include "donk/interpreter/interpreter.h"
#include "pollux/core/manager/core_manager.h"
#include "pollux/networking/constants.h"
#include "pollux/networking/net.pb.h"
#include "pollux/networking/proto_serialization.h"
#include "pollux/networking/server/server.h"
#include "spdlog/spdlog.h"
#include "type_registrar.h"

namespace pollux::server::states {

void ServerRoom::Run() {
  server_ = std::make_shared<pollux::networking::Server>();
  bool success = server_->Initialize();

  if (!success) {
    spdlog::critical("Unable to initialize server.");
  }

  std::srand(std::time(nullptr));

  StartInterpreter();

  server_->OnConnectedClient(
      [=](enet::Peer* peer, networking::connection_key& ck) {
        this->ConnectClient(peer, ck);
      });

  server_->OnClientReceive([=](enet::Peer* peer, networking::connection_key& ck,
                               networking::proto::PLXMessage& plx) {
    this->ClientReceive(peer, ck, plx);
  });
  server_->OnClientSend([=](enet::Peer* peer, networking::connection_key& ck,
                            networking::proto::PLXMessage& plx) {
    this->ClientSend(peer, ck, plx);
  });
  current_phase_ = pollux::state::GameStatePhase::kHealthy;
}

void ServerRoom::ConnectClient(enet::Peer* peer,
                               networking::connection_key& ck) {
  auto client = interpreter_->Make<::donk::api::client::client_coretype>(
      donk::path_t("/client"));
  interpreter_->EcsManager()->Register(client);
  donk::proc_args_t empty;
  interpreter_->QueueSpawn(
      [client, ck](donk::proc_ctxt_t& ctxt,
                   donk::proc_args_t& args) -> donk::running_proc {
        co_yield ctxt.ChildProc(client, "New", {});
        *client->v("key") = ck.username;
        *client->v("mob")->v("key") = ck.username;
        co_yield ctxt.ChildProc(client->v("mob")->get_iota(), "Login", {});
        spdlog::info("Made client key: {}", ck.username);
        ctxt.GetEcsManager()->Register(client->v("mob")->get_iota());
        co_return;
      },
      empty);
  interpreter_->GetWorld()->clients_by_username_[ck.username] = client;
  networking::proto::PLXMessage plxMessage;
  networking::ProtoSerializers::EncodeClientConnect(*interpreter_->EcsManager(),
                                                    plxMessage);
  server_->DeliverMessage(plxMessage, peer);
}

void ServerRoom::HandleInput(float delta) {}

void ServerRoom::Update(float delta) {
  if (interpreter_->Active()) {
    interpreter_->Tick();
    networking::proto::PLXMessage plxMessage;
    int count = 0;
    while (!interpreter_->GetWorld()->internal_broadcast_log_.empty()) {
      auto broadcast = plxMessage.add_broadcasts();
      broadcast->set_data(interpreter_->GetWorld()->PopBroadcast());
      count++;
    }

    if (count > 0) {
      for (auto [peer, _] : server_->clients()) {
        server_->DeliverMessage(plxMessage, peer);
      }
    }
  }

  server_->Update(delta);
}

void ServerRoom::ClientReceive(enet::Peer*, networking::connection_key& ck,
                               networking::proto::PLXMessage& plx_message) {
  // TODO: Obviously a more robust command parser is needed but not yet it isn't
  auto client = interpreter_->GetWorld()->clients_by_username_[ck.username];
  for (auto client_input : plx_message.client_inputs()) {
    std::string s(client_input.command());
    std::string delimiter = " \"";
    std::vector<std::string> command;
    // Shamelessly stolen from https://stackoverflow.com/questions/14265581
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
      token = s.substr(0, pos);
      command.push_back(token);
      s.erase(0, pos + delimiter.length());
    }

    if (command.size() >= 1) {
      auto first_token = command[0];
      auto has_proc = client->v("mob")->get_iota()->HasProc(first_token);
      if (!has_proc) {
        spdlog::info("Discarding invalid client command `{}`",
                     client_input.command());
        continue;
      }
    }

    if (command.size() == 1) {
      spdlog::info("Client Input: `{}`", command[0]);
      donk::proc_args_t empty;
      interpreter_->QueueProc(client->v("mob")->get_iota(), command[0], empty);
    } else if (command.size() == 2) {
      spdlog::info("Client Input: `{}` `{}`", command[0], command[1]);
      donk::proc_args_t args(command[1]);
      interpreter_->QueueProc(client->v("mob")->get_iota(), command[0], args);
    } else {
      spdlog::warn("could not split input `{}`", client_input.command());
      continue;
    }
  }
}

void ServerRoom::ClientSend(enet::Peer*, networking::connection_key&,
                            networking::proto::PLXMessage&) {}

void ServerRoom::StartInterpreter() {
  auto collector = std::make_shared<std::map<
      donk::path_t, std::vector<std::function<void(donk::iota_t&)>>>>();
  dtpo::RegisterAll(collector);
  (*collector)[donk::path_t("/")].push_back(donk::api::Register);

  auto environment = core_manager_->GetResourceManager()->GetEnvironment();

  interpreter_ = donk::internal::Interpreter::Create();
  interpreter_->SetEnvironment(environment);
  interpreter_->SetRegistrationFunctions(collector);
  interpreter_->RegisterCoreprocs();
  interpreter_->CreateWorld();
  interpreter_->ResetMaps();
  interpreter_->Run();
}

std::string ServerRoom::GetWindowName() {
  return "Donk Server " + time_as_string_;
}

}  // namespace pollux::server::states
