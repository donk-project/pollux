#include "pollux/backends/gwork_sdl2/backend.h"

#include "Gwork/Controls/Canvas.h"
#include "Gwork/Skins/TexturedBase.h"
#include "SDL.h"
#include "SDL2pp/Point.hh"
#include "SDL2pp/Rect.hh"
#include "SDL2pp/Renderer.hh"
#include "SDL2pp/SDL.hh"
#include "SDL2pp/Window.hh"
#include "pollux/core/manager/core_manager.h"
#include "pollux/geom/vector.h"
#include "pollux/render/texture.h"
#include "SDL_ttf.h"
#include "pollux/ui/manager/ui_manager.h"
#include "spdlog/spdlog.h"

namespace pollux {
namespace backends {

GworkSDL2OpenGLBackend::GworkSDL2OpenGLBackend(
    std::shared_ptr<core::CoreManager> core_manager)
    : core_manager_(core_manager) {}

std::shared_ptr<render::Texture> GworkSDL2OpenGLBackend::MakeTexture() {
  auto texture = std::make_shared<render::Texture>();
  return texture;
}

bool GworkSDL2OpenGLBackend::IsOpen() { return is_open_; }

void GworkSDL2OpenGLBackend::CloseWindow() { is_open_ = false; }

void GworkSDL2OpenGLBackend::BuildWindow(std::string name) {
  int width = 1024;
  int height = 768;
  sdl_ = std::make_unique<SDL2pp::SDL>(SDL_INIT_VIDEO);
  window_ = std::make_unique<SDL2pp::Window>(
      name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
      /*SDL_WINDOW_OPENGL |*/ SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN |
          SDL_WINDOW_RESIZABLE);
  renderer_ = std::make_unique<SDL2pp::Renderer>(*window_, -1,
                                                 SDL_RENDERER_ACCELERATED);

  if (TTF_Init() != 0) {
    spdlog::critical("Unable to load SDL_ttf: {}", TTF_GetError());
  }

  core_manager_->GetResourceManager()->Initialize();

  gwork_renderer_ =
      std::make_shared<Gwk::Renderer::SDL2>(*this, window_->Get());
  skin_ = std::make_shared<Gwk::Skin::TexturedBase>(gwork_renderer_.get());
  skin_->SetRender(gwork_renderer_.get());
  skin_->Init("DefaultSkin.png");
  skin_->SetDefaultFont("OpenSans.ttf", 16);

  gwork_canvas_ = std::make_shared<Gwk::Controls::Canvas>(skin_.get());
  gwork_canvas_->SetSize(width, height);
  gwork_canvas_->SetDrawBackground(true);
  gwork_canvas_->SetBackgroundColor(Gwk::Color(150, 170, 170, 255));

  device_scale_factor = renderer_->GetOutputHeight() / window_->GetHeight();
  gwork_canvas_->SetScale(device_scale_factor);
  
  gwork_input_ = std::make_unique<Gwk::Input::SDL2>();
  gwork_input_->Initialize(gwork_canvas_.get());

  renderer_->SetDrawColor(0, 16, 32).Clear();
  renderer_->Present();

  SDL_SetRenderDrawBlendMode(renderer_->Get(), SDL_BLENDMODE_BLEND);
  is_open_ = true;
}

bool GworkSDL2OpenGLBackend::Poll(pollux::Event& event) { return event.Poll(); }

Gwk::String GworkSDL2OpenGLBackend::GetPath(Gwk::ResourcePaths::Type type,
                                            Gwk::String const& name) {
  return core_manager_->GetResourceManager()->GetApplicationResourcePath(name);
}

std::shared_ptr<Gwk::Renderer::SDL2>
GworkSDL2OpenGLBackend::GetGworkSDL2Renderer() {
  return gwork_renderer_;
}

std::shared_ptr<Gwk::Controls::Canvas>
GworkSDL2OpenGLBackend::GetGworkSDL2Canvas() {
  return gwork_canvas_;
}

void GworkSDL2OpenGLBackend::HandleEvent(pollux::Event& event) {
  gwork_input_->ProcessEvent(&event.sdl_event_);
  if (event.sdl_event_.type == SDL_WINDOWEVENT &&
      event.sdl_event_.window.event == SDL_WINDOWEVENT_RESIZED) {
    int w = event.sdl_event_.window.data1;
    int h = event.sdl_event_.window.data2;
    gwork_canvas_->SetSize(w, h);
    if (current_ui_state_ != nullptr) {
      current_ui_state_->OnResize(w, h);
    }
  }
}

void GworkSDL2OpenGLBackend::ShutdownAll() {}

void GworkSDL2OpenGLBackend::Setup(std::shared_ptr<pollux::ui::UIState> state) {
  if (current_ui_state_ != nullptr) {
    current_ui_state_->OnTeardown();
  }

  current_ui_state_ = state;
  current_ui_state_->OnSetup();
}

void GworkSDL2OpenGLBackend::PreDraw() {
  renderer_->SetDrawColor(0, 0, 0).Clear();
  gwork_renderer_->BeginContext(nullptr);
}

void GworkSDL2OpenGLBackend::Draw() {
  gwork_canvas_->RenderCanvas();
}

void GworkSDL2OpenGLBackend::PostDraw() {
  gwork_renderer_->PresentContext(nullptr);
  gwork_renderer_->EndContext(nullptr);
}

float GworkSDL2OpenGLBackend::GetScalingFactor() const {
  return device_scale_factor;
}

geom::Vector2u GworkSDL2OpenGLBackend::render_dimensions() {
  // FIXME:
  return geom::Vector2u(0, 0);
}

bool GworkSDL2OpenGLBackend::HasTexture(std::string filename) {
  for (auto& pair : icon_sheets_) {
    if (pair.first == filename) {
      return true;
    }
  }
  return false;
}

void GworkSDL2OpenGLBackend::RenderIcon(donk::image::dmi_data_t& icon_data,
                                        std::string state_name,
                                        geom::Vector2u screenPos) {
  auto frame = core_manager_->GetRenderManager()->IconStateFrame(
      icon_data, state_name, 0);
  std::shared_ptr<SDL2pp::Texture> icon_sheet;
  if (HasTexture(icon_data.source_filename)) {
    icon_sheet = icon_sheets_[icon_data.source_filename];
  } else {
    auto fullpath = icon_data.source_filename;
    auto filename =
        core_manager_->GetResourceManager()->GetDMResourcePath(fullpath);
    icon_sheet = std::make_shared<SDL2pp::Texture>(*renderer_, filename);
    icon_sheets_[icon_data.source_filename] = icon_sheet;
  }
  SDL2pp::Rect sourceRect{frame.x(), frame.y(), frame.w(), frame.h()};
  SDL2pp::Rect dstRect{
      // FIXME: This x-y offset is proportional to the map tile dimensions, not
      // icon
      SDL2pp::Point{static_cast<int>(icon_data.icon_width * screenPos.x() *
                                     iconRenderScale),
                    static_cast<int>(icon_data.icon_height * screenPos.y() *
                                     iconRenderScale)},
      SDL2pp::Point{static_cast<int>(icon_data.icon_width * iconRenderScale),
                    static_cast<int>(icon_data.icon_height * iconRenderScale)}};

  renderer_->Copy(*icon_sheet, sourceRect, dstRect);
}

geom::Rect GworkSDL2OpenGLBackend::IconStateFrame(
    donk::image::dmi_data_t& dmi_data, std::string state_name, int frame) {
  int idx = -1;
  for (int i = 0; i < dmi_data.states.size(); i++) {
    if (dmi_data.states[i].name == state_name) {
      idx = i;
    }
  }
  if (idx == -1) {
    throw std::runtime_error(
        fmt::format("could not find state {} in data for {}", state_name,
                    dmi_data.source_filename));
  }
  auto state = dmi_data.states[idx];
  if (frame >= state.frames) {
    throw std::runtime_error(
        fmt::format("asked for frame {} of {}/{} but only {} frames exist",
                    frame, dmi_data.source_filename, state_name, state.frames));
  }
  int icons_per_row = dmi_data.image_width / dmi_data.icon_width;
  geom::Rect rect;
  rect.w(dmi_data.icon_width);
  rect.h(dmi_data.icon_height);
  rect.x((idx + frame) % icons_per_row * dmi_data.icon_height);
  rect.y((idx + frame) / icons_per_row * dmi_data.icon_width);
  return rect;
}


}  // namespace backends
}  // namespace pollux
