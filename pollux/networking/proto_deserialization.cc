// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#include "pollux/networking/proto_deserialization.h"

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
void ProtoDeserializers::DecodeClientConnect(
    proto::PLXMessage& plx_message,
    std::vector<donk::preset_t>& preset_collector,
    std::map<donk::entity_id, std::vector<std::string>>& update_entity_id_varnames) {
  int count = 0;
  spdlog::info("ProtoDeserializers::DecodeClientConnect()");
  for (auto& create : plx_message.entity_creates()) {
    donk::preset_t preset(donk::path_t(create.entity().path().fq()));
    preset.entity_id(create.entity().entity_id());
    for (auto& pair : create.entity().vars().vars()) {
      auto name = pair.first;
      auto val = pair.second;
      auto var = std::make_shared<donk::var_t>();
      if (val.has_i()) {
        // We index emplacements here to avoid conflict between int and
        // donk::entity_id
        var->data_.emplace<0>(val.i());
      } else if (val.has_f()) {
        var->data_.emplace<1>(val.f());
      } else if (val.has_s()) {
        var->data_.emplace<2>(val.s());
      } else if (val.has_entity_id()) {
        var->data_.emplace<3>(val.entity_id());
        update_entity_id_varnames[preset.entity_id()].push_back(name);
      } else if (val.has_resource()) {
        var->data_.emplace<5>(
            std::make_shared<donk::resource_t>(val.resource().name()));
      } else if (val.has_prefab()) {
        auto pr = donk::prefab_t(val.prefab().path().fq());
        var->data_.emplace<6>(pr);
      } else {
        throw std::runtime_error(fmt::format(
            "unable to find value of serialized var {} with contents ``{}``",
            name, val.DebugString()));
      }
      preset.vars().insert_by_name(name, var);
    }
    preset_collector.push_back(preset);
    count++;
  }
  spdlog::info("{} entities updated.", count);
}

}  // namespace networking
}  // namespace pollux
