// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#include "pollux/resources/manager/mac_resource_manager.h"

#include "donk/interpreter/environment/filesystem_environment.h"

#include "SDL.h"

namespace pollux {
namespace resources {

void MacResourceManager::Initialize() {
  bundle_resource_base_path_ = SDL_GetBasePath();
  environment_ = std::make_shared<donk::environment::FilesystemEnvironment>(
      bundle_resource_base_path_ + "snowfrost/snowfrost/FalacyTut");

  environment_->DEBUG__LogFindings();
}

std::string MacResourceManager::GetDMResourcePath(std::string name) {
  return bundle_resource_base_path_ + "snowfrost/snowfrost/FalacyTut/" + name;
}

std::string MacResourceManager::GetApplicationResourcePath(std::string name) {
  return bundle_resource_base_path_ + name;
}

}  // namespace resources
}  // namespace pollux
