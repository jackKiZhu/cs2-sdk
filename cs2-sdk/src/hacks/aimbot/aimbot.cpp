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
    // if (!weapon || !weapon->CanPrimaryAttack(1, 0.f)) return false;
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

    Vector targetAngle;
    perfectAngle = lastMove.viewAngles;

    Vector punch = localPawn->GetLastAimPunch();
    const Vector punchDelta = (punch - oldPunch).NormalizedAngle();
    oldPunch = punch;

    rcsAngle = lastMove.viewAngles - punchDelta * 2.f;
    rcsAngle.z = 0.f;
    rcsAngle.NormalizeAngle();

    curAngle = rcsAngle;

    //curAngle.x -= punchDelta.x * 2.f * g_Vars.m_RecoilX;
    //curAngle.y -= punchDelta.y * 2.f * g_Vars.m_RecoilY;
    //curAngle.z = 0.f;
    //curAngle.NormalizeAngle();

    // if (weapon->m_nNextPrimaryAttackTick() >= localController->m_nTickBase()) return;
    if (weapon->GetAccuracy() > 0.05f) return;

    if (g_Vars.m_EnableTriggerbot && weapon->m_nNextPrimaryAttackTick() < localController->m_nTickBase()) {
        const Vector end = localPos + lastMove.viewAngles.ToVector().Normalized() * 4096.f;
        GameTrace_t trace;
        if (CEngineTrace::Get()->TraceShape(localPos, end, localPawn, 0x1C1003, 4, &trace)) {
            if (trace.hitEntity && trace.hitEntity->IsPlayerPawn()) {
                C_CSPlayerPawn* hitPawn = static_cast<C_CSPlayerPawn*>(trace.hitEntity);

                lastMove.Scroll(IN_ATTACK);

                // if (hitPawn->m_iTeamNum() != hitPawn->m_iTeamNum()) {
                //         if (!(input->moveData.buttonsHeld & IN_ATTACK)) input->buttonsChanged |= IN_ATTACK;
                // input->buttonsHeld |= IN_ATTACK;
                //}
            }
        }
    }

    const std::lock_guard<std::mutex> lock(CMatchCache::GetLock());

    const auto& cachedEntities = CMatchCache::GetCachedEntities();
    CCachedPlayer* target = nullptr;

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

        const int playerIndex = cachedPlayer->GetIndex();
        if (trace.fraction < 0.97f) {
            visibleSince[playerIndex] = 0.f;
            continue;
        }

        visibleSince[playerIndex] += CGlobalVars::Get()->intervalPerTick;

        if (!IsVisible(playerIndex, 0.15f)) continue;

        Vector angle = CMath::Get().CalculateAngle(localPos, pos);
        const float fov = CMath::Get().Fov(rcsAngle, angle);
        if (fov < currentFov) {
            target = cachedPlayer;
            targetAngle = angle;
            currentFov = fov;
        }
    }

    const bool inputDown = lastMove.buttonsHeld & IN_ATTACK || lastMove.buttonsHeld & IN_ATTACK2;
    if (inputDown) lastActiveTime = CGlobalVars::Get()->currentTime;

    const float mouseLength = std::hypot(lastMove.mouseDx, lastMove.mouseDy);

    const bool shouldAim = currentFov <= g_Vars.m_AimFov && CGlobalVars::Get()->currentTime - lastActiveTime <= 0.2f;

    if (target && shouldAim) {
        lastMove.viewAngles = curAngle + Smooth(rcsAngle, targetAngle - punch * 2);
    } else if (localPawn->m_iShotsFired() > 1) {
        // print the different angles
        CLogger::Log("curAngle: {} {}, rcsAngle: {} {}, targetAngle: {} {}", curAngle.x, curAngle.y, rcsAngle.x, rcsAngle.y, targetAngle.x,
                     targetAngle.y);

        lastMove.viewAngles = curAngle;
        pid[0].Reset();
        pid[1].Reset();
    }

    lastMove.viewAngles.NormalizeAngle();
}

bool CAimbot::IsVisible(int index, float for_) {
    if (!visibleSince.count(index)) return false;
    return visibleSince[index] > for_;
}

Vector CAimbot::RCS(const Vector& angles, C_CSPlayerPawn* pawn, float factor) {
    static Vector prevPunch = {};
    const Vector punch = pawn->GetLastAimPunch();
    const Vector delta = (punch - prevPunch).NormalizedAngle();
    prevPunch = punch;

    if (punch.IsZero()) return angles;
    if (pawn->m_iShotsFired() <= 1) return angles;
    return (angles - delta * 2.f * factor).NormalizedAngle();
}

Vector CAimbot::Smooth(const Vector& from, const Vector& to) {
    const PIDConfig_t pidCfg{.m_KP = g_Vars.m_KP * 0.3f, .m_KI = g_Vars.m_KI * 0.3f, .m_kd = 0.f, .m_Damp = 1.f - g_Vars.m_Damp};

    Vector delta = (to - from).NormalizedAngle();
    delta.x = pid[0].Step(delta.x, 0.015f, pidCfg);
    delta.y = pid[1].Step(delta.y, 0.015f, pidCfg);
    return delta;
}
