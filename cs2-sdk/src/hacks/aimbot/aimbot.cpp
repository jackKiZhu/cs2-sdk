#include "pch.hpp"
#include <hacks/aimbot/aimbot.hpp>

#include <cache/cache.hpp>
#include <vars/vars.hpp>

#include <interfaces/engineclient.hpp>
#include <interfaces/enginetrace.hpp>
#include <interfaces/ccsgoinput.hpp>
#include <interfaces/globalvars.hpp>

#include <cache/entities/player.hpp>
#include <bindings/playercontroller.hpp>
#include <bindings/playerpawn.hpp>
#include <bindings/weapon.hpp>

#include <math/math.hpp>

#include <logger/logger.hpp>

bool CAimbot::IsEnabled() { 
    if (!g_Vars.m_EnableAimbot || !CEngineClient::Get()->IsInGame()) return false;
    CCachedPlayer* cachedLocal = CMatchCache::Get().GetLocalPlayer();
    if (!cachedLocal || !cachedLocal->IsValid()) return false;
    CCSPlayerController* localController = cachedLocal->Get();
    C_CSPlayerPawnBase* localPawn = localController->m_hPawn().Get();
    C_CSWeaponBaseGun* weapon = localPawn->GetActiveWeapon();
    //if (!weapon || !weapon->CanPrimaryAttack(1, 0.f)) return false;
    if (weapon->m_nNextPrimaryAttackTick() >= localController->m_nTickBase()) return false;
    return true;
}

void CAimbot::Run() {
    CCSGOInput* input = CCSGOInput::Get();
    if (!IsEnabled()) return;

    CCachedPlayer* cachedLocal = CMatchCache::Get().GetLocalPlayer();
    CCSPlayerController* localController = cachedLocal->Get();
    C_CSPlayerPawn* localPawn = localController->m_hPawn().Get();
    C_BasePlayerWeapon* weapon = localPawn->GetActiveWeapon();
    Vector localPos;
    localPawn->GetEyePos(&localPos);

    if (g_Vars.m_EnableTriggerbot) {
        const Vector end = localPos + input->viewAngles.ToVector().Normalized() * 4096.f;
        GameTrace_t trace;
        if (CEngineTrace::Get()->TraceShape(localPos, end, localPawn, 0x1C1003, 4, &trace)) {
            if (trace.hitEntity && trace.hitEntity->IsPlayerPawn()) {
                C_CSPlayerPawn* hitPawn = static_cast<C_CSPlayerPawn*>(trace.hitEntity);
                //if (hitPawn->m_iTeamNum() != hitPawn->m_iTeamNum()) {
                if (!(input->buttonsHeld & IN_ATTACK)) input->buttonsChanged |= IN_ATTACK;
			    input->buttonsHeld |= IN_ATTACK;
				//}
            }
        }
    }
    

    const std::lock_guard<std::mutex> lock(CMatchCache::GetLock());

    const auto& cachedEntities = CMatchCache::GetCachedEntities();
    CCachedPlayer* target = nullptr;

    RCS(input->viewAngles, localPawn);

    Vector aimAngle = input->viewAngles;

    float currentFov = std::numeric_limits<float>::max();
    for (const auto& it : cachedEntities) {
        const auto& cachedEntity = it.second;
        if (!cachedEntity->IsValid() || cachedEntity->GetType() != CCachedBaseEntity::Type::PLAYER) continue;

        CCachedPlayer* cachedPlayer = dynamic_cast<CCachedPlayer*>(cachedEntity.get());
        if (!cachedPlayer || !cachedPlayer->IsEnemyWithTeam(cachedLocal->GetTeam())) continue;

        CCSPlayerController* controller = cachedPlayer->Get();
        if (!controller->m_bPawnIsAlive()) continue;

        C_CSPlayerPawn* pawn = controller->m_hPawn().Get();
        if (!pawn) continue;

        Vector pos;
        if (!pawn->GetHitboxPosition(0, pos)) continue;

        GameTrace_t trace;
        if (!CEngineTrace::Get()->TraceShape(localPos, pos, localPawn, 0x1C1003, 4, &trace)) continue;
        if (trace.fraction < 0.97f) continue;  // visibility

        const Vector angle = CMath::Get().CalculateAngle(localPos, pos);
        const float fov = CMath::Get().Fov(input->viewAngles, angle);
        if (fov < currentFov) {
            target = cachedPlayer;
            aimAngle = angle;
            currentFov = fov;
        }
    }

    const bool shouldAim = currentFov <= g_Vars.m_aimFov && 
        (input->buttonsHeld & IN_ATTACK || input->buttonsHeld & IN_ATTACK2);

    if (target && shouldAim) {
        input->viewAngles = Smooth(input->viewAngles, aimAngle);


    }
}

void CAimbot::RCS(Vector& angles, C_CSPlayerPawn* pawn) { 
    static Vector prevPunch = {};
    const CUtlVector<Vector>& cache = pawn->m_aimPunchCache();
    if (cache.m_Size <= 0) return;

    const Vector& punch = cache.At(cache.m_Size - 1);
    const Vector delta = (punch - prevPunch).NormalizedAngle();
    prevPunch = punch;

    if (punch.IsZero()) return;
    if (pawn->m_iShotsFired() <= 1) return;

    CLogger::Log("Punch: {} {} {}", punch.x, punch.y, punch.z);

    angles -= delta;
    angles.NormalizeAngle();
}

Vector CAimbot::Smooth(const Vector& from, const Vector& to) { 
    const Vector delta = (to - from).NormalizedAngle(); 
    return (from + delta * g_Vars.m_aimSmooth).NormalizedAngle();
}


