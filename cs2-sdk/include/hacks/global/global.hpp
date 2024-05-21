#pragma once

#include <math/types/vector.hpp>

#include <cache/cache.hpp>
#include <cache/entities/player.hpp>

#include <interfaces/ccsgoinput.hpp>

#include <bindings/playercontroller.hpp>
#include <bindings/playerpawn.hpp>
#include <bindings/weapon.hpp>
#include <bindings/vdata.hpp>

class CGlobal {
   public:
    static CGlobal& Get() {
        static CGlobal inst;
        return inst;
    }

    bool Update(CUserCmd* cmd);

    CUserCmd* cmd;
    CCachedPlayer* player;
    CCSPlayerController* controller;
    C_CSPlayerPawn* pawn;
    C_CSWeaponBaseGun* weapon;
    CCSWeaponBaseVData* vdata;
    Vector aimPunch;
    Vector punchDelta;
    Vector eyePos;
    Vector viewAngles;
    Vector rcsAngles;
};
