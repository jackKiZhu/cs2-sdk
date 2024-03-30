#include "pch.hpp"
#include <hacks/aimbot/aimbot.hpp>

#include <cache/cache.hpp>
#include <vars/vars.hpp>

#include <interfaces/engineclient.hpp>
#include <interfaces/enginetrace.hpp>
#include <input/ccsgoinput.hpp>

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
    C_BasePlayerWeapon* weapon = localPawn->GetActiveWeapon();
    if (!weapon /*|| !weapon->CanFire()*/) return false;
    return true;
}

void CAimbot::Run(CCSGOInput* input) {
    if (!IsEnabled()) return;

    CCachedPlayer* cachedLocal = CMatchCache::Get().GetLocalPlayer();
    CCSPlayerController* localController = cachedLocal->Get();
    C_CSPlayerPawn* localPawn = localController->m_hPawn().Get();
    Vector localPos;
    localPawn->GetEyePos(&localPos);

    C_BasePlayerWeapon* weapon = localPawn->GetActiveWeapon();



    const std::lock_guard<std::mutex> lock(CMatchCache::GetLock());

    const auto& cachedEntities = CMatchCache::GetCachedEntities();
    CCachedPlayer* target = nullptr;
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

    RCS(aimAngle, localPawn);

    if (target) {
        input->viewAngles = aimAngle;
    }
}

void CAimbot::RCS(Vector& angles, C_CSPlayerPawn* pawn) { 
    static Vector prevPunch = {};
    CUtlVector<Vector>& cache = pawn->m_aimPunchCache();
    if (cache.m_Size <= 0) return;

    const Vector& punch = cache.At(cache.m_Size - 1);
    const Vector delta = (punch - prevPunch).NormalizeAngle_();
    prevPunch = punch;

    if (punch.IsZero()) return;
    if (pawn->m_iShotsFired() <= 1) return;

    CLogger::Log("Punch: {} {} {}", punch.x, punch.y, punch.z);

    angles -= delta;
    angles.NormalizeAngle();
}

