// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#include "pollux/client/states/client_room.h"

#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "donk/core/vars.h"
#include "donk/interpreter/ecs_manager.h"
#include "entt/core/algorithm.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entity/group.hpp"
#include "entt/entity/sparse_set.hpp"
#include "entt/entity/storage.hpp"
#include "pollux/networking/client/client.h"
#include "pollux/networking/constants.h"
#include "pollux/networking/net.pb.h"
#include "pollux/networking/proto_deserialization.h"
#include "pollux/resources/manager/resource_manager.h"
#include "pollux/ui/manager/ui_manager.h"
#include "pollux/ui/states/client/client_login_ui.h"
#include "pollux/ui/states/client/primary_client_ui.h"
#include "spdlog/fmt/bundled/format.h"
#include "spdlog/spdlog.h"

namespace pollux::client::states {

static bool CanBeDrawn(donk::var_table_t& var_table) {
  // spdlog::info("CanBeDrawn:");
  // var_table.DEBUG__PrintVarTable();
  // spdlog::info("");
  auto a = var_table.HasTypedVar<std::shared_ptr<donk::resource_t>>("icon");
  auto b = var_table.HasTypedVar<std::string>("icon_state");
  auto c = b && var_table.Lookup("icon_state")->get_string() != "";

  return a && c;
}

void ClientRoom::Run() {
  auto login_ui = pollux::ui::MakeClientLoginUI(core_manager_);
  login_ui->OnAttemptConnection([=]() -> bool {
    networking::connection_key nc;
    nc.ip = login_ui->GetIpAddress();
    nc.port = 23655;
    nc.username = login_ui->GetUsername();
    nc.client_state = networking::ClientState::kNew;

    return AttemptConnection(nc);
  });
  core_manager_->GetUIManager()->Setup(login_ui);
  current_phase_ = pollux::state::GameStatePhase::kHealthy;
}

void ClientRoom::OnLogin() {}

void ClientRoom::OnBroadcast(std::string s) {
  // chatBox_->addLine(partial_packet_.str());
}

void ClientRoom::AttemptRender(donk::var_table_t& var_table) {
  if (core_manager_->GetResourceManager()->GetEnvironment()->GetIconCount() ==
      0) {
    spdlog::critical("No icon data found but SpriteFromState requested.");
    return;
  }

  auto icon_name = var_table.Lookup("icon")->get_resource()->name();
  auto icon_state = var_table.Lookup("icon_state")->get_string();

  if (!core_manager_->GetResourceManager()->GetEnvironment()->HasIcon(
          icon_name)) {
    spdlog::critical("Unable to find icon file {}.", icon_name);
  }

  auto& icon_data =
      *core_manager_->GetResourceManager()->GetEnvironment()->GetIconData(
          icon_name);

  geom::Vector2u spritePosition(0, 0);

  if (var_table.HasTypedVar<donk::entity_id>("loc")) {
    spdlog::critical(
        "UUID not remapped after sync, something has gone very wrong");
  } else if (var_table.HasTypedVar<std::shared_ptr<donk::iota_t>>("loc")) {
    auto loc = var_table.Lookup("loc")->get_iota();
    auto x = loc->v("x")->get_int();
    auto y = loc->v("y")->get_int();
    spritePosition.x(x - 1);
    spritePosition.y(y - 1);
  } else if (var_table.HasTypedVar<int>("x") &&
             var_table.HasTypedVar<int>("y")) {
    auto x = var_table.Lookup("x")->get_int();
    auto y = var_table.Lookup("y")->get_int();
    spritePosition.x(x - 1);
    spritePosition.y(y - 1);
  } else {
    spdlog::warn("Icon could not be rendered in a safe position");
    // Could not find a location to draw the sprite in.
    // Should probably do something here.
    return;
  }

  core_manager_->GetRenderManager()->RenderIcon(icon_data, icon_state,
                                                spritePosition);
}

void ClientRoom::OnUpdateFrame(networking::proto::PLXMessage& message) {
  std::vector<donk::preset_t> preset_collector;
  std::map<donk::entity_id, std::vector<std::string>> update_entity_id_varnames;
  pollux::networking::ProtoDeserializers::DecodeClientConnect(
      message, preset_collector, update_entity_id_varnames);
  if (!preset_collector.empty()) {
    // spdlog::info("Decode: {}", message.DebugString());
    for (auto& preset : preset_collector) {
      // No UUID means make it and merge in the preset.
      if (preset.entity_id() == 0) {
        auto iota = interpreter_->MakeArbitrary(preset.path().fq());
        iota->Apply(preset);
        // TODO: Pretty sure this needs to be registered here as well
      } else {
        // If there's a UUID and we know about it, simply merge.
        if (interpreter_->EcsManager()->IsRegistered(preset.entity_id())) {
          interpreter_->EcsManager()->Merge(preset);
        } else {
          // Otherwise, we need to construct the iota and register it.
          // At this point we can no longer call EcsManager::Register,
          // because we've received UUIDs from somewhere else and can't
          // increment our own.

          // Need the iota's proc_table but don't really need anything else.
          auto iota = interpreter_->MakeArbitrary(preset.path().fq());
          interpreter_->EcsManager()->RegisterManually(iota, preset);
        }
      }
    }
    interpreter_->UpdateUuidLinks(update_entity_id_varnames);
    sort_required_ = true;
  }

  if (message.broadcasts_size() > 0) {
    for (auto& broadcast : message.broadcasts()) {
      core_manager_->GetBrowserManager()->ExecuteJavascript(
          fmt::format("document.write(\"{}<br />\");", broadcast.data()));
    }
  }

  return;
}

void ClientRoom::HandleInput(float delta) {}

void ClientRoom::Update(float delta) {
  if (client_ != nullptr && client_->is_connected()) {
    client_->GetNotifications();
  }
}

void ClientRoom::Draw() {
  if (interpreter_ == nullptr) {
    return;
  }
  auto group = interpreter_->EcsManager()
                   ->registry()
                   .group<donk::path_t, std::shared_ptr<donk::iota_t>>();
  if (sort_required_) {
    // When sorting is required, the group are arranged in rendering order.
    // TODO: This doesn't belong here and is terribly naive.
    group.sort([this](const entt::entity lentity, const entt::entity rentity) {
      const auto& lhs =
          interpreter_->EcsManager()->registry().get<donk::path_t>(lentity);
      const auto& rhs =
          interpreter_->EcsManager()->registry().get<donk::path_t>(rentity);
      // spdlog::info("LHS {} RHS {}", lhs, rhs);
      if (lhs.IsArea()) {
        if (rhs.IsArea()) {
          return lhs.fq() < rhs.fq();
        }
        if (rhs.IsTurf()) {
          return true;
        }
        if (rhs.IsObj()) {
          return true;
        }
        if (rhs.IsMob()) {
          return true;
        }
      } else if (lhs.IsTurf()) {
        if (rhs.IsArea()) {
          return false;
        }
        if (rhs.IsTurf()) {
          return lhs.fq() < rhs.fq();
        }
        if (rhs.IsObj()) {
          return true;
        }
        if (rhs.IsMob()) {
          return true;
        }
      } else if (lhs.IsObj()) {
        if (rhs.IsArea()) {
          return false;
        }
        if (rhs.IsTurf()) {
          return false;
        }
        if (rhs.IsObj()) {
          return true;
        }
        if (rhs.IsMob()) {
          return true;
        }
      } else if (lhs.IsMob()) {
        if (rhs.IsArea()) {
          return false;
        }
        if (rhs.IsTurf()) {
          return false;
        }
        if (rhs.IsObj()) {
          return false;
        }
        if (rhs.IsMob()) {
          return lhs.fq() < rhs.fq();
        }
      }
      return lhs.fq() < rhs.fq();
    });
    sort_required_ = false;
  }

  for (auto entity : group) {
    auto iota = group.get<std::shared_ptr<donk::iota_t>>(entity);
    auto& path = group.get<donk::path_t>(entity);
    if (client::states::CanBeDrawn(iota->var_table())) {
      AttemptRender(iota->var_table());
    }
  }
}

bool ClientRoom::AttemptConnection(networking::connection_key nc) {
  if (client_ != nullptr && client_->is_connected()) {
    spdlog::warn("Already connected.");
    return false;
  }

  client_ = std::make_shared<networking::Client>(nc);
  auto result = client_->Connect();
  if (result) {
    spdlog::info("Connected to host {}:{}.", nc.ip, nc.port);
    StartInterpreter();
    auto primary_client_ui = pollux::ui::MakePrimaryClientUI(core_manager_);
    primary_client_ui->OnChatboxEnter(
        [=](const std::string& s) { sendPlayerTextInput(s); });
    client_->OnLogin([=]() { this->OnLogin(); });
    client_->OnBroadcast([=](std::string s) { this->OnBroadcast(s); });
    client_->OnUpdateFrame(
        [=](networking::proto::PLXMessage& m) { this->OnUpdateFrame(m); });
    core_manager_->GetUIManager()->Setup(primary_client_ui);
    if (core_manager_->GetRenderManager()->GetScalingFactor() > 1.0) {
      core_manager_->GetBrowserManager()->EnableHighDPI(
          core_manager_->GetRenderManager()->GetScalingFactor());
    }
    auto browserRect = primary_client_ui->GetSuggestedBrowserRect();
    core_manager_->GetBrowserManager()->SetOnScreenPosition(browserRect);
  }
  return result;
}

void ClientRoom::sendPlayerTextInput(std::string input) {
  spdlog::info("Sending player input `{}`", input);
  client_->SendUserCommand(input);
}

std::string ClientRoom::GetWindowName() {
  return "Donk Client " + time_as_string_;
}

}  // namespace pollux::client::states
