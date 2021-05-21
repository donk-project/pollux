// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __SNOWFROST_POLLUX_RESOURCES_DEFAULT_RESOURCE_MANAGER_H__
#define __SNOWFROST_POLLUX_RESOURCES_DEFAULT_RESOURCE_MANAGER_H__

#include <map>
#include <memory>
#include <string>

#include "donk/core/vars.h"
#include "donk/image/dmi_parser.h"
#include "pollux/core/manager/core_manager.h"
#include "pollux/geom/rect.h"
#include "pollux/render/sprite.h"
#include "pollux/render/texture.h"
#include "pollux/resources/manager/resource_manager.h"
#include "tools/cpp/runfiles/runfiles.h"

using bazel::tools::cpp::runfiles::Runfiles;

namespace pollux {
namespace resources {

class DefaultResourceManager : public ResourceManager {
 public:
  DefaultResourceManager(std::shared_ptr<core::CoreManager>, std::string);

  void Initialize() override;

  std::string GetDMResourcePath(std::string name) override;
  std::string GetApplicationResourcePath(std::string name) override;
  std::shared_ptr<donk::Environment> GetEnvironment() override {
    return environment_;
  }

 private:
  std::shared_ptr<core::CoreManager> core_manager_;
  std::shared_ptr<donk::Environment> environment_;
  std::string resource_path_;
  bazel::tools::cpp::runfiles::Runfiles* runfiles_;
};

}  // namespace resources
}  // namespace pollux

#endif  // __SNOWFROST_POLLUX_RESOURCES_DEFAULT_RESOURCE_MANAGER_H__
