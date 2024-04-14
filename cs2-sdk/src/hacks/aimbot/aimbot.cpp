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
    if (!localController) return false;
    C_CSPlayerPawnBase* localPawn = localController->m_hPawn().Get();
    if (!localPawn) return false;
    C_CSWeaponBaseGun* weapon = localPawn->GetActiveWeapon();
    if (!weapon) return false;
    //if (!weapon || !weapon->CanPrimaryAttack(1, 0.f)) return false;
    if (weapon->m_nNextPrimaryAttackTick() >= localController->m_nTickBase()) return false;
    return true;
}

void CAimbot::Run(CMoveData* moveData) {
    if (!IsEnabled()) return;

    CMoveData& lastMove = *moveData;
    CCachedPlayer* cachedLocal = CMatchCache::Get().GetLocalPlayer();
    CCSPlayerController* localController = cachedLocal->Get();
    C_CSPlayerPawn* localPawn = localController->m_hPawn().Get();
    C_BasePlayerWeapon* weapon = localPawn->GetActiveWeapon();
    Vector localPos;
    localPawn->GetEyePos(&localPos);

    RCS(lastMove.viewAngles, localPawn);

    if (weapon->GetAccuracy() > 0.05f) return;

    if (g_Vars.m_EnableTriggerbot) {
        const Vector end = localPos + lastMove.viewAngles.ToVector().Normalized() * 4096.f;
        GameTrace_t trace;
        if (CEngineTrace::Get()->TraceShape(localPos, end, localPawn, 0x1C1003, 4, &trace)) {
            if (trace.hitEntity && trace.hitEntity->IsPlayerPawn()) {
                C_CSPlayerPawn* hitPawn = static_cast<C_CSPlayerPawn*>(trace.hitEntity);

                lastMove.Scroll(IN_ATTACK);

                //if (hitPawn->m_iTeamNum() != hitPawn->m_iTeamNum()) {
       //         if (!(input->moveData.buttonsHeld & IN_ATTACK)) input->buttonsChanged |= IN_ATTACK;
			    //input->buttonsHeld |= IN_ATTACK;
				//}
            }
        }
    }
    
    const std::lock_guard<std::mutex> lock(CMatchCache::GetLock());

    const auto& cachedEntities = CMatchCache::GetCachedEntities();
    CCachedPlayer* target = nullptr;

    Vector aimAngle = lastMove.viewAngles;

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
        if (trace.fraction < 0.97f) continue; // visibility

        const Vector angle = CMath::Get().CalculateAngle(localPos, pos);
        const float fov = CMath::Get().Fov(lastMove.viewAngles, angle);
        if (fov < currentFov) {
            target = cachedPlayer;
            aimAngle = angle;
            currentFov = fov;
        }
    }

    const float mouseLength = std::hypot(lastMove.mouseDx, lastMove.mouseDy);

    const bool shouldAim = currentFov <= g_Vars.m_AimFov && 
        (lastMove.buttonsHeld & IN_ATTACK || lastMove.buttonsHeld & IN_ATTACK2);

    if (target && shouldAim) {
        lastMove.viewAngles = Smooth(lastMove.viewAngles, aimAngle);
        // CLogger::Log("Mouse: {} {} {}", lastMove.mouseDx, lastMove.mouseDy, mouseLength);
        CLogger::Log("Inaccuracy: {}", weapon->GetAccuracy());
    }
    else
    {
        pid[0].Reset();
        pid[1].Reset();
    }
}

void CAimbot::RCS(Vector& angles, C_CSPlayerPawn* pawn) { 
    static Vector prevPunch = {};
    CUtlVector<Vector>& cache = pawn->m_aimPunchCache();
    int cacheIndex = cache.m_Size - 1;
    if (cacheIndex < 0) return;
    const Vector& punch = cache.At(cacheIndex);
    const Vector delta = (punch - prevPunch).NormalizedAngle();
    prevPunch = punch;

    if (punch.IsZero()) return;
    if (pawn->m_iShotsFired() <= 1) return;

    CLogger::Log("Punch: {} {} {}", punch.x, punch.y, punch.z);

    angles -= delta;
    angles.NormalizeAngle();
}

Vector CAimbot::Smooth(const Vector& from, const Vector& to) { 
    const PIDConfig_t pidCfg { 
        .m_KP = g_Vars.m_KP * 0.3f, 
        .m_KI = g_Vars.m_KI * 0.3f, 
        .m_kd = 0.f, 
        .m_Damp = 1.f - g_Vars.m_Damp
    };

    Vector delta = (to - from).NormalizedAngle(); 
    delta.x = pid[0].Step(delta.x, 0.015f, pidCfg);
    delta.y = pid[1].Step(delta.y, 0.015f, pidCfg);
    return (from + delta).NormalizedAngle();
}
