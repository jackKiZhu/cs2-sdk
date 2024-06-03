#pragma once

namespace sdl {
    int SetRelativeMouseMode(int enabled);
    int SetWindowMouseGrab(void* window, int grabbed);
    void WarpMouseInWindow(void* window, float x, float y);
}  // namespace sdl
