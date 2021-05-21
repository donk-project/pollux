// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __SNOWFROST_POLLUX_NETWORKING_PROTO_DESERIALIZATION_H__
#define __SNOWFROST_POLLUX_NETWORKING_PROTO_DESERIALIZATION_H__

#include <map>
#include <string>
#include <vector>

#include "donk/core/iota.h"
#include "donk/core/path.h"
#include "donk/core/vars.h"
#include "donk/interpreter/ecs_manager.h"
#include "google/protobuf/text_format.h"
#include "pollux/networking/net.pb.h"
#include "spdlog/spdlog.h"

namespace pollux {
namespace networking {
  
class ProtoDeserializers {
 public:
  static void DecodeClientConnect(
      proto::PLXMessage& plx_message,
      std::vector<donk::preset_t>& preset_collector,
      std::map<donk::entity_id, std::vector<std::string>>& update_entity_id_varnames);
};

}  // namespace networking
}  // namespace pollux

#endif  // __SNOWFROST_POLLUX_NETWORKING_PROTO_DESERIALIZATION_H__