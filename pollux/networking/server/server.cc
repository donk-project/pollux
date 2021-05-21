// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#include "pollux/networking/server/server.h"

#include "enet-plus/enet.h"
#include "enet-plus/event.h"
#include "enet-plus/peer.h"
#include "enet-plus/server_host.h"
#include "pollux/networking/constants.h"
#include "pollux/networking/net.pb.h"
#include "spdlog/spdlog.h"

// Fuck Microsoft
#undef CreateEvent

namespace pollux::networking {

bool Server::Initialize() {
  enet_ = new enet::Enet;
  bool rv = enet_->Initialize();
  if (!rv) {
    spdlog::error("Unable to initialize network.");
    return false;
  }

  server_host_ = enet_->CreateServerHost(23655);
  if (server_host_ == nullptr) {
    spdlog::error("Unable to initialize server host.");
    return false;
  }

  return true;
}

void Server::DeliverMessage(proto::PLXMessage& plxMessage, enet::Peer* peer) {
  size_t size = plxMessage.ByteSizeLong();
  char* buffer = (char*)malloc(size);
  bool result = plxMessage.SerializeToArray(buffer, size);
  if (!result) {
    throw std::runtime_error("unable to serialize plxmessage");
  }
  result = peer->Send(buffer, size, true);
  if (!result) {
    spdlog::error("Failed to send plx message to peer: {}",
                  plxMessage.DebugString());
  }
}

void Server::Update(float delta) {
  auto event = enet_->CreateEvent();
  bool rv = server_host_->Service(event, 100);
  if (!rv) {
    spdlog::critical("Event service failed.");
  }

  switch (event->GetType()) {
    case enet::Event::TYPE_CONNECT: {
      // TODO: See if already connected
      auto ck = std::make_shared<networking::connection_key>();
      ck->client_state = ClientState::kNew;
      clients_[event->GetPeer()] = ck;
      // *client->networking_client_
      //     << pollux::networking::PacketPrefix::kClientConnectUpdate <<
      //     output;

      // proto::PLXMessage other_clients_update;
      // pollux::networking::ProtoSerializers::EncodeBasicCreate(
      //     interpreter_->EcsManager(), other_clients_update,
      //     {client_entity_id, mob_entity_id});
      // result = other_clients_update.SerializeToString(&output);

      // if (!result) {
      //   throw std::runtime_error("unable to serialize other_clients_update");
      // }
      // for (auto& other : interpreter_->GetWorld()->clients_) {
      //   if (client == other) {
      //     continue;
      //   }
      //   *other->networking_client_
      //       << pollux::networking::PacketPrefix::kClientConnectUpdate <<
      //       output;

      break;
    }

    case enet::Event::TYPE_RECEIVE: {
      auto search = clients_.find(event->GetPeer());
      if (search != clients_.end()) {
        auto ck = search->second;
        if (ck->client_state == ClientState::kNew) {
          std::vector<char> output;
          event->GetData(&output);
          char* data(&output[0]);
          proto::LoginRequest login_request;
          bool success = login_request.ParseFromArray(data, output.size());
          if (!success) {
            spdlog::error(
                "Expected a login request but could not parse one. Got {}",
                data);
            break;
          }
          ck->client_state = ClientState::kAuthenticated;
          ck->ip = event->GetPeer()->GetIp();
          ck->port = event->GetPeer()->GetPort();
          ck->username = login_request.username();
          if (on_connected_client_ != nullptr) {
            on_connected_client_(event->GetPeer(), *ck);
          }
        } else if (ck->client_state == ClientState::kAuthenticated) {
          std::vector<char> output;
          event->GetData(&output);
          char* data(&output[0]);
          proto::PLXMessage plx_message;
          bool success = plx_message.ParseFromArray(data, output.size());
          if (!success) {
            spdlog::error(
                "Expected a plx message but could not parse one. Got {}", data);
            break;
          }
          if (on_client_receive_ != nullptr) {
            on_client_receive_(event->GetPeer(), *ck, plx_message);
          }
        }
      }

      break;
    }

    case enet::Event::TYPE_DISCONNECT: {
      spdlog::info("Disconnect");
      break;
    }
  }

  // while (!interpreter_->GetWorld()->internal_broadcast_log_.empty()) {
  //   auto s = interpreter_->GetWorld()->PopBroadcast();
  //   for (auto client : interpreter_->GetWorld()->clients_) {
  //     if (client->v("mob")->IsKind<std::shared_ptr<donk::iota_t>>()) {
  //       *client->networking_client_
  //           << pollux::networking::PacketPrefix::kClientReceiveBroadcast <<
  //           s;
  //     }
  //   }
  // }
}

}  // namespace pollux::networking