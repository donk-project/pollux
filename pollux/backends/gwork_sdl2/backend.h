#ifndef __POLLUX_POLLUX_BACKENDS_GWORK_SDL2_BACKEND_H__
#define __POLLUX_POLLUX_BACKENDS_GWORK_SDL2_BACKEND_H__

#include "Gwork/Input/SDL2.h"
#include "Gwork/PlatformTypes.h"
#include "Gwork/Skins/TexturedBase.h"
#include "SDL.h"
#include "SDL2pp/Renderer.hh"
#include "SDL2pp/SDL.hh"
#include "SDL2pp/Window.hh"
#include "pollux/backends/gwork.h"
#include "pollux/core/manager/core_manager.h"
#include "pollux/geom/vector.h"
#include "pollux/render/texture.h"
#include "pollux/ui/manager/ui_manager.h"
#include "spdlog/spdlog.h"

namespace pollux {
namespace ui {
class UIState;
class PrimaryClientUI;
class ClientLoginUI;
}  // namespace ui

namespace backends {

struct GworkSDL2OpenGLBackend_Impl;

// GworkSDL2OpenGLBackend combines a ui::UIManager and a render::RenderManager
// in order to handle complicated tight coupling between SDL2, Metal, and
// Gwork, as well as manage Objective-C objects independently of the rest of
// the implementation.
class GworkSDL2OpenGLBackend : public ui::UIManager,
                               public render::RenderManager,
                               public GworkBackend,
                               public Gwk::ResourcePaths {
 public:
  GworkSDL2OpenGLBackend(std::shared_ptr<core::CoreManager> core_manager);

  // render::RenderManager
  std::shared_ptr<render::Texture> MakeTexture() override;
  void BuildWindow(std::string name) override;
  bool IsOpen() override;
  void CloseWindow() override;
  void PreDraw() override;
  void PostDraw() override;
  float GetScalingFactor() const override;
  geom::Vector2u render_dimensions() override;
  void RenderIcon(donk::image::dmi_data_t& icon_data, std::string state_name,
                  geom::Vector2u screenPos) override;
  geom::Rect IconStateFrame(donk::image::dmi_data_t& dmi_data,
                            std::string state_name, int frame) override;

  // ui::UIManager
  void Setup(std::shared_ptr<ui::UIState> state) override;
  void Draw() override;
  bool Poll(pollux::Event& event) override;
  void HandleEvent(pollux::Event& event) override;
  void ShutdownAll() override;

  // Gwk::ResourcePaths
  Gwk::String GetPath(Gwk::ResourcePaths::Type type,
                      Gwk::String const& name) final;

  // GworkBackend
  std::shared_ptr<Gwk::Controls::Canvas> GetGworkSDL2Canvas() override;
  std::shared_ptr<Gwk::Renderer::SDL2> GetGworkSDL2Renderer() override;

  SDL_Renderer* GetSDL2Renderer() { return renderer_->Get(); }

 private:
  bool HasTexture(std::string filename);

  std::shared_ptr<core::CoreManager> core_manager_;
  std::shared_ptr<ui::UIState> current_ui_state_;

  std::unique_ptr<SDL2pp::SDL> sdl_;
  std::unique_ptr<SDL2pp::Window> window_;
  std::unique_ptr<SDL2pp::Renderer> renderer_;

  std::shared_ptr<Gwk::Renderer::SDL2> gwork_renderer_;
  std::shared_ptr<Gwk::Controls::Canvas> gwork_canvas_;
  std::shared_ptr<Gwk::Skin::TexturedBase> skin_;
  std::unique_ptr<Gwk::Input::SDL2> gwork_input_;

  bool is_open_;

  std::map<std::string, std::shared_ptr<SDL2pp::Texture>> icon_sheets_;

  float iconRenderScale = 2.0f;
  float device_scale_factor = 1.0f;

  friend class PrimaryClientUI;
  friend class ClientLoginUI;
};

}  // namespace backends
}  // namespace pollux

#endif  // __POLLUX_POLLUX_BACKENDS_GWORK_SDL2_BACKEND_H__
