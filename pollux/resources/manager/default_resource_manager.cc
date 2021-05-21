// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#include "pollux/resources/manager/default_resource_manager.h"

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "donk/core/iota.h"
#include "donk/core/vars.h"
#include "donk/image/dmi_parser.h"
#include "donk/interpreter/environment/runfiles_environment.h"
#include "fmt/format.h"
#include "pollux/core/manager/core_manager.h"
#include "pollux/geom/rect.h"

namespace pollux {
namespace resources {

DefaultResourceManager::DefaultResourceManager(
    std::shared_ptr<core::CoreManager> core_manager, std::string resource_path)
    : core_manager_(core_manager), resource_path_(resource_path) {}

void DefaultResourceManager::Initialize() {
  environment_ = std::make_shared<donk::environment::RunfilesEnvironment>(
      resource_path_, "snowfrost/snowfrost/FalacyTut/");

  environment_->DEBUG__LogFindings();

  std::string error;
  runfiles_ = Runfiles::Create(resource_path_, &error);

  if (runfiles_ == nullptr) {
    spdlog::error("Error loading runfiles: {}", error);
    std::exit(201);
  }
}

namespace {
// Shamelessly stolen from https://stackoverflow.com/questions/874134
inline bool ends_with(std::string const& value, std::string const& ending) {
  if (ending.size() > value.size()) return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}
}  // namespace

std::string DefaultResourceManager::GetDMResourcePath(std::string name) {
  if (ends_with(name, ".dmi")) {
    return environment_->GetFullIconPath(name);
  }
  spdlog::critical("GetDMResourcePath({}) has unsupported extension", name);
  return "";
}

std::string DefaultResourceManager::GetApplicationResourcePath(
    std::string name) {
  return runfiles_->Rlocation("pollux/pollux/assets/" + name);
}

}  // namespace resources
}  // namespace pollux
