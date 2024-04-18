#pragma once

#include <math/types/vector.hpp>
#include <hacks/aimbot/pid.hpp>
#include <interfaces/ccsgoinput.hpp>

class C_CSPlayerPawn;

class CAimbot {
   public:
    static CAimbot& Get() {
        static CAimbot inst;
        return inst;
    }

    bool IsEnabled();
    void Run(CMoveData* moveData);
    void Draw();

    bool IsVisible(int index, float for_ = 0.f);

   private:

    Vector RCS(const Vector& angles, C_CSPlayerPawn* pawn, float factor = 1.f);
    Vector Smooth(const Vector& from, const Vector& to);

    // the previous target that was aimed at
    CCachedPlayer* oldTarget = nullptr;
    // the initial switch time
    float lastTargetSwitchTime = 0.f;

    // controllers for smooth
    PIDController_t pid[2];

    float lastActiveTime = 0.f;
    
    // previous (sub)tick punch
    Vector oldPunch;

    // the angle that would hit the enemy with 100% accuracy
    Vector perfectAngle;
    // the angle with current view angles with perfect recoil
    Vector rcsAngle;
    // the angle with current view angles with settings recoil
    Vector curAngle;

    std::unordered_map<int, float> visibleSince;
};
