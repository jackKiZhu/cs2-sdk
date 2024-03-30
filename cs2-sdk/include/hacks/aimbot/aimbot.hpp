#pragma once

#include <math/types/vector.hpp>

class CCSGOInput;
class C_CSPlayerPawn;

class CAimbot {
   public:
    static CAimbot& Get() {
        static CAimbot inst;
        return inst;
    }

    bool IsEnabled();

    void Run(CCSGOInput* input);
    void RCS(Vector& angles, C_CSPlayerPawn* pawn);
};
