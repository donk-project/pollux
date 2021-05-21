// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_RESOURCES_MANAGER_MAC_RESOURCE_MANAGER_H__
#define __POLLUX_POLLUX_RESOURCES_MANAGER_MAC_RESOURCE_MANAGER_H__

#include <string>

#include "donk/core/vars.h"
#include "donk/image/dmi_parser.h"
#include "donk/image/dmi_reader.h"
#include "pollux/geom/rect.h"
#include "pollux/render/sprite.h"
#include "pollux/resources/manager/resource_manager.h"

namespace pollux {
namespace resources {

class MacResourceManager : public ResourceManager {
 public:
  MacResourceManager(std::shared_ptr<core::CoreManager> core_manager,
                     std::string resource_path)
      : core_manager_(core_manager) {}

  void Initialize() override;
  std::string GetDMResourcePath(std::string name) override;
  std::string GetApplicationResourcePath(std::string name) override;
  std::shared_ptr<donk::Environment> GetEnvironment() override {
    return environment_;
  }

 private:
  std::shared_ptr<core::CoreManager> core_manager_;
  std::shared_ptr<donk::Environment> environment_;
  std::string bundle_resource_base_path_;
};

}  // namespace resources
}  // namespace pollux

#endif  // __POLLUX_POLLUX_RESOURCES_MANAGER_MAC_RESOURCE_MANAGER_H__
