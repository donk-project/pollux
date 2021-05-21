// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __SNOWFROST_POLLUX_RESOURCES_RESOURCE_MANAGER_H__
#define __SNOWFROST_POLLUX_RESOURCES_RESOURCE_MANAGER_H__

#include <string>

#include "donk/core/vars.h"
#include "donk/image/dmi_parser.h"
#include "donk/image/dmi_reader.h"
#include "donk/interpreter/environment/environment.h"
#include "pollux/geom/rect.h"
#include "pollux/render/sprite.h"

namespace pollux {
namespace core {
class CoreManager;
}  // namespace core

namespace resources {

class ResourceManager {
 public:
  virtual ~ResourceManager() {}
  virtual void Initialize() = 0;

  virtual std::string GetDMResourcePath(std::string name) = 0;
  virtual std::string GetApplicationResourcePath(std::string name) = 0;

  virtual std::shared_ptr<donk::Environment> GetEnvironment() = 0;
};

}  // namespace resources
}  // namespace pollux

#endif  // __SNOWFROST_POLLUX_RESOURCES_RESOURCE_MANAGER_H__
