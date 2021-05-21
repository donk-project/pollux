#ifndef __POLLUX_POLLUX_BROWSER_MANAGER_BROWSER_MANAGER_H__
#define __POLLUX_POLLUX_BROWSER_MANAGER_BROWSER_MANAGER_H__

#include "pollux/common/event.h"
#include "pollux/geom/rect.h"

namespace pollux {
namespace browser {

struct WorldLogOutputBrowser {};

struct ModalWindowBrowser {};

class BrowserManager {
  public:
    virtual void Configure() = 0;
    virtual bool Initialize() = 0;
    virtual void HandleEvent(pollux::Event&) = 0;
    virtual void Work() = 0;
    virtual void Render() = 0;
    virtual void Cleanup() = 0;
    virtual bool CanAcceptEvents() = 0;
    virtual bool CanRender() = 0;
    virtual void ExecuteJavascript(std::string s) = 0;
    virtual void SetOnScreenPosition(geom::Rect rect) = 0;
    virtual void EnableHighDPI(float scaling_factor) = 0;
};

}  // namespace browser
}  // namespace pollux

#endif // __POLLUX_POLLUX_BROWSER_MANAGER_BROWSER_MANAGER_H__
