#pragma once

class CInputSystem {
   public:
    static CInputSystem* Get();

    bool IsMouseLocked();
    void* GetSDLWindow();
};
