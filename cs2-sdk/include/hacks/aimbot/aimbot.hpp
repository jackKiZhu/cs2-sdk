#pragma once

#include <deque>

#include <hacks/aimbot/pid.hpp>
#include <hacks/aimbot/lagcomp.hpp>

#include <interfaces/ccsgoinput.hpp>

#include <cache/cache.hpp>

#include <imgui/imgui_internal.h> // vec2

#include <math/types/vector.hpp>

class C_CSPlayerPawn;

class CAimbot {
   public:
    static CAimbot& Get() {
        static CAimbot inst;
        return inst;
    }

    bool IsEnabled();
    void Run(const TargetData_t& data);

    void Update();
    void Render();
    void Test(CCSGOInputHistoryEntryPB* historyEntry);

    bool IsInSmoke(const Vector& start, const Vector& end);

   private:

    void Invalidate();

    bool shouldAim = false;

    Vector Smooth(const Vector& from, const Vector& to);

    // the current target that is being aimed at
    CCachedPlayer* target = nullptr;
    // the previous target that was aimed at
    CCachedPlayer* oldTarget = nullptr;
    // the initial switch time
    float lastTargetSwitchTime = 0.f;

    // controllers for smooth
    PIDController_t pid[2];

    float lastActiveTime = 0.f;

    // the angle that would hit the enemy with 100% accuracy
    Vector perfectAngle;
    // the angle with current view angles with settings recoil
    Vector curAngle;

    // draw data
    ImVec2 targetScreen;
};
