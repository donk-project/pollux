// Donk Project
// Copyright (c) 2021 Warriorstar Orion <orion@snowfrost.garden>
// SPDX-License-Identifier: MIT
#ifndef __POLLUX_POLLUX_RENDER_RENDER_MANAGER_H__
#define __POLLUX_POLLUX_RENDER_RENDER_MANAGER_H__

#include "donk/image/dmi_parser.h"
#include "pollux/common/event.h"
#include "pollux/render/texture.h"
#include "pollux/geom/rect.h"

namespace pollux {
namespace core {
class CoreManager;
}  // namespace core

namespace render {

class RenderManager {
 public:
  virtual std::shared_ptr<Texture> MakeTexture() = 0;
  virtual void BuildWindow(std::string name) = 0;
  virtual bool IsOpen() = 0;
  virtual void CloseWindow() = 0;
  virtual void PreDraw() = 0;
  virtual void PostDraw() = 0;
  virtual float GetScalingFactor() const = 0;
  virtual geom::Vector2u render_dimensions() = 0;
  virtual void RenderIcon(donk::image::dmi_data_t& icon_data,
                          std::string state_name, geom::Vector2u screenPos) = 0;
  virtual geom::Rect IconStateFrame(donk::image::dmi_data_t& dmi_data,
                                    std::string state_name, int frame) = 0;
};

}  // namespace render
}  // namespace pollux

#endif  // __POLLUX_POLLUX_RENDER_RENDER_MANAGER_H__
