// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __SNOWFROST_POLLUX_NETWORK_PROTO_SERIALIZATION_H__
#define __SNOWFROST_POLLUX_NETWORK_PROTO_SERIALIZATION_H__

#include <algorithm>

#include "donk/core/iota.h"
#include "donk/core/path.h"
#include "donk/core/vars.h"
#include "donk/interpreter/ecs_manager.h"
#include "pollux/networking/net.pb.h"
#include "spdlog/spdlog.h"

namespace pollux {
namespace networking {

class ProtoSerializers {
 public:
  static void EncodeClientConnect(donk::ecs::EcsManager& manager,
                                  proto::PLXMessage& plx_message);
  static void EncodeBasicCreate(donk::ecs::EcsManager& manager,
                                proto::PLXMessage& plx_message,
                                std::vector<donk::entity_id> entity_ids);
  static void EncodeVarTable(donk::var_table_t& var_table,
                             proto::Iota& iota_msg);
  static void EncodeVar(donk::var_t& var, proto::Var& var_msg);
  static void EncodePreset(donk::preset_t& preset, proto::Preset& pb);
  static void EncodeAssocList(donk::assoc_list_t& assoc_list, proto::AssocList& pb);
};

}  // namespace networking
}  // namespace pollux

#endif  // __SNOWFROST_POLLUX_NEtWORK_PROTO_SERIALIZATION_H__