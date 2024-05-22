#pragma once

#include <hook/hook.hpp>

class CGameHooks {
   public:
    static CGameHooks& Get() {
        static CGameHooks inst;
        return inst;
    }

    void Initialize();

    CHook g_DrawObject;
};
