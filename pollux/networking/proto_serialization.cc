// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#include "pollux/networking/proto_serialization.h"

#include <algorithm>
#include <map>

#include "donk/core/iota.h"
#include "donk/core/path.h"
#include "donk/core/vars.h"
#include "donk/interpreter/ecs_manager.h"
#include "pollux/networking/net.pb.h"
#include "spdlog/spdlog.h"

namespace pollux {
namespace networking {

void ProtoSerializers::EncodeClientConnect(donk::ecs::EcsManager& manager,
                                           proto::PLXMessage& plx_message) {
  auto view =
      manager.registry_
          .view<donk::entity_id, donk::path_t, std::shared_ptr<donk::iota_t>>();
  for (auto entity : view) {
    auto& entity_id = view.get<donk::entity_id>(entity);
    auto& path = view.get<donk::path_t>(entity);
    auto& var_table =
        view.get<std::shared_ptr<donk::iota_t>>(entity)->var_table();

    auto create = plx_message.add_entity_creates();
    create->mutable_entity()->mutable_path()->set_fq(path.fq());
    create->mutable_entity()->set_entity_id(entity_id);
    EncodeVarTable(var_table, *create->mutable_entity());
  }
}

void ProtoSerializers::EncodeBasicCreate(donk::ecs::EcsManager& manager,
                                         proto::PLXMessage& plx_message,
                                         std::vector<donk::entity_id> entity_ids) {
  auto view =
      manager.registry_
          .view<donk::entity_id, donk::path_t, std::shared_ptr<donk::iota_t>>();
  for (auto entity : view) {
    auto& entity_id = view.get<donk::entity_id>(entity);
    if (std::find(std::begin(entity_ids), std::end(entity_ids), entity_id) !=
        std::end(entity_ids)) {
      auto& path = view.get<donk::path_t>(entity);
      auto& var_table =
          view.get<std::shared_ptr<donk::iota_t>>(entity)->var_table();

      auto create = plx_message.add_entity_creates();
      create->mutable_entity()->mutable_path()->set_fq(path.fq());
      create->mutable_entity()->set_entity_id(entity_id);
      EncodeVarTable(var_table, *create->mutable_entity());
    }
  }
}

void ProtoSerializers::EncodeAssocList(donk::assoc_list_t& assoc_list,
                                       proto::AssocList& pb) {
  std::map<std::string, std::shared_ptr<donk::var_t>>::iterator it;
  for (it = assoc_list.vars_by_name().begin();
       it != assoc_list.vars_by_name().end(); it++) {
    proto::Var var;
    EncodeVar(*(it->second), var);
    auto& map = *pb.mutable_vars();
    map[it->first] = var;
  }
}

void ProtoSerializers::EncodePreset(donk::preset_t& preset, proto::Preset& pb) {
  pb.mutable_path()->set_fq(preset.path().fq());
  if (preset.entity_id() > 0) {
    pb.set_entity_id(preset.entity_id());
  }
  EncodeAssocList(preset.vars(), *pb.mutable_vars());
}

void ProtoSerializers::EncodeVarTable(donk::var_table_t& var_table,
                                      proto::Iota& iota_msg) {
  for (auto& [name, var] : *var_table.vars_) {
    proto::Var var_msg;
    EncodeVar(*var, var_msg);
    auto& map = *iota_msg.mutable_vars()->mutable_vars();
    map[name] = var_msg;
  }
}

void ProtoSerializers::EncodeVar(donk::var_t& var, proto::Var& var_msg) {
  if (var.IsKind<int>()) {
    var_msg.set_i(var.get_int());
  } else if (var.IsKind<std::string>()) {
    var_msg.set_s(var.get_string());
  } else if (var.IsKind<float>()) {
    var_msg.set_f(var.get_float());
  } else if (var.IsKind<std::shared_ptr<donk::iota_t>>()) {
    var_msg.set_entity_id(var.get_iota()->entity_id_);
  } else if (var.IsKind<std::shared_ptr<donk::resource_t>>()) {
    var_msg.mutable_resource()->set_name(var.get_resource()->name());
  } else if (var.IsKind<donk::prefab_t>()) {
    proto::Prefab* prefab = new proto::Prefab;
    prefab->mutable_path()->set_fq(var.get_prefab().path().fq());
    var_msg.set_allocated_prefab(prefab);
  } else {
    spdlog::error("ProtoSerializers::EncodeVar() can't convert {}", var);
  }
}

}  // namespace networking
}  // namespace pollux