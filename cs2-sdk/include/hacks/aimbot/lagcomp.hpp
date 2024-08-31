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
    void Reset() {
        data.player = nullptr;
        data.controller = nullptr;
        data.pawn = nullptr;
        data.records = nullptr;
        data.bestRecord = nullptr;
        data.bestRecordIndex = 0;
        data.bestBone = 6;
        data.aimPos = {};
    }
    void PreModifyInput(CInputFrame* msg, CCSGOInputHistoryEntryPB* historyEntry);
    void PostModifyInput(CInputFrame* msg, CCSGOInputHistoryEntryPB* historyEntry);
    void Test();

    std::tuple<float, float, float> GetSubtickSimtime();
    float GetClientInterp();

    TargetData_t data;

   private:
    bool calculatedInterpolations;
    bool restore = false;
    CInputFrame backup;
    InterpInfo_t cl, sv0, sv1;
};
