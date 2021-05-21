// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#include "pollux/networking/client/client.h"

#include <functional>
#include <vector>

#include "enet-plus/client_host.h"
#include "enet-plus/enet.h"
#include "enet-plus/event.h"
#include "enet-plus/peer.h"
#include "pollux/networking/constants.h"
#include "pollux/networking/net.pb.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/spdlog.h"

// Fuck Microsoft
#undef CreateEvent

namespace pollux {
namespace networking {

void Client::OnLogin(std::function<void()> f) { on_login_ = f; }

void Client::OnBroadcast(std::function<void(std::string)> f) {
  on_broadcast_ = f;
}

void Client::OnUpdateFrame(std::function<void(proto::PLXMessage&)> f) {
  on_update_frame_ = f;
}

bool Client::Connect() {
  enet_ = new enet::Enet;
  bool rv = enet_->Initialize();
  if (!rv) {
    spdlog::critical("Unable to initialize network.");
  }

  client_ = enet_->CreateClientHost();
  if (client_ == NULL) {
    spdlog::critical("Unable to initialize client host.");
  }

  peer_ = client_->Connect(connection_key_.ip, connection_key_.port);
  if (peer_ == NULL) {
    spdlog::error("Unable to connect to server {}:{}.", connection_key_.ip,
                  connection_key_.port);
  }

  auto event = enet_->CreateEvent();
  uint32_t timeout = 100;

  int n_tries = 0;
  int backoff = 100;
  while (n_tries < 5 && !is_connected_) {
    rv = client_->Service(event, backoff);
    if (!rv) {
      spdlog::error("Event service failed.");
      return false;
    }
    if (event->GetType() == enet::Event::TYPE_CONNECT) {
      spdlog::info("Connected to {}:{}.", event->GetPeer()->GetIp(),
                   event->GetPeer()->GetPort());
      is_connected_ = true;
    } else {
      backoff *= 2;
      n_tries++;
      spdlog::info("No connection. Backoff is now {}ms.", backoff);
    }
  }

  if (!is_connected_) {
    spdlog::error("Unable to connect after {} tries.", n_tries);
    return false;
  }

  proto::LoginRequest login_request;
  login_request.set_username(connection_key_.username);
  size_t size = login_request.ByteSizeLong();
  char* buffer = (char*)malloc(size);

  auto result = login_request.SerializeToArray(buffer, size);
  if (!result) {
    spdlog::critical("Unable to serialize login request");
  }

  rv = peer_->Send(buffer, size, true);
  if (!rv) {
    spdlog::error("Failed to send login message.");
    return false;
  }

  return true;
}

void Client::SendUserCommand(std::string input) {
  proto::PLXMessage message;
  auto client_input = message.add_client_inputs();
  client_input->set_command(input);

  size_t size = message.ByteSizeLong();
  char *buffer = (char*)malloc(size);

  auto result = message.SerializeToArray(buffer, size);
  if (!result) {
    spdlog::critical("Unable to serialize command input");
  }
  auto rv = peer_->Send(buffer, size, true);
  if (!rv) {
    spdlog::error("Failed to send command input.");
  }
}

void Client::Disconnect() {
  auto event = enet_->CreateEvent();
  peer_->Disconnect();
  bool rv = client_->Service(event, 0);
  if (!rv) {
    spdlog::error("Failed to properly disconnect.");
  }
  if (event->GetType() != enet::Event::TYPE_DISCONNECT) {
    spdlog::error("Failed to acknowledge disconnect.");
  }
}

void Client::GetNotifications() {
  auto event = enet_->CreateEvent();
  bool rv = client_->Service(event, 0);
  if (!rv) {
    spdlog::error("Failed to service client event.");
  }

  switch (event->GetType()) {
    case enet::Event::TYPE_RECEIVE: {
      std::vector<char> output;
      event->GetData(&output);
      char* data(&output[0]);
      spdlog::info("Received message.");
      proto::PLXMessage message;
      bool result = message.ParseFromArray(data, output.size());
      if (!result) {
        spdlog::error("Failed to parse PLX Message, got: {}", data);
      }
      if (on_update_frame_ != nullptr) {
        on_update_frame_(message);
      }
    }
  }

  // if (status == sf::Socket::Done) {
  //   if (mode_ == pollux::networking::ClientMode::kLoginAccepted) {
  //     int uid;
  //     if (packet >> uid) {
  //       spdlog::info("ClientMode::kLoginAccepted connected as client {}",
  //       uid);
  //     }
  //     id_ = uid;
  //     is_connected_ = true;
  //     if (on_login_ != nullptr) {
  //       on_login_(id_);
  //     }
  //     mode_ = pollux::networking::ClientMode::kWaiting;
  //     return;
  //   }
  //   if (mode_ == pollux::networking::ClientMode::kReceivingUpdateFrame) {
  //     spdlog::info(
  //         "ClientMode::kReceivingUpdateFrame received, current mode "
  //         "{}.",
  //         mode_);
  //     std::string s;
  //     if (packet >> s) {
  //       partial_packet_ << s;
  //       spdlog::info("Received Connect Update");
  //       proto::PLXMessage message;
  //       bool result = message.ParseFromString(partial_packet_.str());
  //       if (!result) {
  //         throw std::runtime_error("connect update failed to parse");
  //       }
  //       if (on_update_frame_ != nullptr) {
  //         on_update_frame_(message);
  //       }
  //       mode_ = pollux::networking::ClientMode::kWaiting;
  //     }
  //   }
  //   if (mode_ == pollux::networking::ClientMode::kReceivingBroadcast) {
  //     spdlog::info("ClientMode::kReceivingBroadcast received, current mode
  //     {}.",
  //                  mode_);
  //     std::string s;
  //     if (packet >> s) {
  //       partial_packet_ << s;
  //       if (on_broadcast_ != nullptr) {
  //         on_broadcast_(partial_packet_.str());
  //       }
  //       std::stringstream swap;
  //       partial_packet_.swap(swap);
  //     }
  //     mode_ = pollux::networking::ClientMode::kWaiting;
  //     return;
  //   }

  //   int code;
  //   if (packet >> code) {
  //     spdlog::info("ClientRoom::PacketCode(): {}", code);
  //     switch (static_cast<pollux::networking::PacketPrefix>(code)) {
  //       case pollux::networking::PacketPrefix::kLoginSuccess: {
  //         spdlog::info("PacketPrefix::kLoginSuccess received, current mode
  //         {}.",
  //                      mode_);
  //         mode_ = pollux::networking::ClientMode::kWaiting;
  //         break;
  //       }
  //       case pollux::networking::PacketPrefix::kClientReceiveBroadcast: {
  //         spdlog::info(
  //             "PacketPrefix::kClientReceiveBroadcast received, current mode "
  //             "{}.",
  //             mode_);
  //         mode_ = pollux::networking::ClientMode::kReceivingBroadcast;
  //         break;
  //       }
  //       case pollux::networking::PacketPrefix::kClientConnectUpdate: {
  //         spdlog::info(
  //             "PacketPrefix::kClientConnectUpdate received, current mode
  //             {}.", mode_);
  //         mode_ = pollux::networking::ClientMode::kReceivingUpdateFrame;
  //       }
  //     }
  //   }
  //   return;
  // }
  // if (status == sf::Socket::Disconnected) {
  //   spdlog::info("Server disconnected.");
  //   is_connected_ = false;
  //   return;
  // }
  // if (status == sf::Socket::Error) {
  //   spdlog::error("Error retrieving packet.");
  //   return;
  // }
}

}  // namespace networking
}  // namespace pollux