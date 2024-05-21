#pragma once

#include <vector>
#include <deque>

#include <math/types/vector.hpp>

#include <hacks/aimbot/pid.hpp>

#include <interfaces/ccsgoinput.hpp>

#include <types/model.hpp>
#include <math/types/vector.hpp>
#include <bindings/playercontroller.hpp>
#include <bindings/playerpawn.hpp>

#include <cache/cache.hpp>

class CRecord {
   public:
    CRecord(C_CSPlayerPawn* pawn) { Store(pawn); }
    void Store(C_CSPlayerPawn* pawn);
    void Apply(C_CSPlayerPawn* pawn, bool clean = true);
    bool IsValid() const { return valid; }

    bool valid;
    std::vector<BoneData_t> boneMatrix;

    int flags;
    float simulationTime;
    Vector rotation;
    Vector absRotation;
    Vector velocity;
    Vector absVelocity;
    Vector eyeAngles;
    Vector origin;
    Vector absOrigin;
    Vector eyePos;

    Vector mins;
    Vector maxs;
};

struct TargetData_t {
    CCachedPlayer* player = nullptr;
    CCSPlayerController* controller = nullptr;
    C_CSPlayerPawn* pawn = nullptr;
    std::deque<CRecord>* records = nullptr;
    CRecord* bestRecord = nullptr;
    size_t bestRecordIndex = 0;
    int bestBone = 6;
    Vector aimPos = {};
};

class CLagComp {
   public:
    static CLagComp& Get() {
        static CLagComp inst;
        return inst;
    }

    TargetData_t Find();
    void Update();

    float LastValidSimtime();
    std::tuple<float, float, float> GetOptimalSimtime();

    TargetData_t data;
};
