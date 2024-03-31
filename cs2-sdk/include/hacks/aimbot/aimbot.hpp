#pragma once

#include <math/types/vector.hpp>

class C_CSPlayerPawn;

class CAimbot {
   public:
    static CAimbot& Get() {
        static CAimbot inst;
        return inst;
    }

    bool IsEnabled();

    void Run();
    void RCS(Vector& angles, C_CSPlayerPawn* pawn);
    Vector Smooth(const Vector& from, const Vector& to);
};
