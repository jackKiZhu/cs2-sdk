#pragma once

#include <interfaces/ccsgoinput.hpp>

// helper class to handle cs2 input system (and subticks)

class CInputHandler {
   public:
    static CInputHandler& Get() {
        static CInputHandler inst;
        return inst;
    }

    void Poll(CCSGOInput* input);
    void Press(uint64_t button, float when = 0.5f);
    bool Release(uint64_t button);
    void Dump();

    bool IsHeld(uint64_t button) const { return buttonsHeld & button; }
    bool IsPressed(uint64_t button) const { return IsScrolled(button) || (IsHeld(button) && !(prevButtonsHeld & button)); }
    bool IsReleased(uint64_t button) const { return IsScrolled(button) || (!IsHeld(button) && (prevButtonsHeld & button)); }
    bool IsScrolled(uint64_t button) const { return buttonsScroll & button; }

   private:
    CMoveData* move; // not needed but convenient

    uint64_t prevButtonsHeld;
    uint64_t buttonsHeld;
    uint64_t buttonsScroll;
};
