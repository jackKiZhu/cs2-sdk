#pragma once

#include <math/types/vector.hpp>

#include <hacks/aimbot/pid.hpp>

#include <interfaces/ccsgoinput.hpp>

#include <cache/cache.hpp>

#include <imgui/imgui_internal.h> // vec2

class C_CSPlayerPawn;

class CAimbot {
   public:
    static CAimbot& Get() {
        static CAimbot inst;
        return inst;
    }

    bool IsEnabled();
    void Run(CUserCmd* cmd);
    void Update();
    void Render();

    bool IsInSmoke(const Vector& start, const Vector& end);

   private:

    void Invalidate();

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

    // current (sub)tick punch
    Vector punch;
    // previous (sub)tick punch
    Vector oldPunch;
    // current punch - previous punch
    Vector punchDelta;

    // the angle that would hit the enemy with 100% accuracy
    Vector perfectAngle;
    // the angle with current view angles with perfect recoil
    Vector rcsAngle;
    // the angle with current view angles with settings recoil
    Vector curAngle;

    // draw data
    ImVec2 targetScreen;
};
