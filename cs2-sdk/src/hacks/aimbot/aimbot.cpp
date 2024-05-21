#include <pch.hpp>

#include <hacks/global/global.hpp>
#include <hacks/aimbot/aimbot.hpp>
#include <hacks/aimbot/lagcomp.hpp>

#include <vars/vars.hpp>

#include <interfaces/engineclient.hpp>
#include <interfaces/enginetrace.hpp>
#include <interfaces/ccsgoinput.hpp>
#include <interfaces/globalvars.hpp>
#include <interfaces/cvar.hpp>
#include <interfaces/cnetworkgameclient.hpp>

#include <cache/entities/player.hpp>
#include <bindings/playercontroller.hpp>
#include <bindings/playerpawn.hpp>
#include <bindings/weapon.hpp>
#include <bindings/gamescenenode.hpp>

#include <math/math.hpp>
#include <math/fitts.hpp>

#include <logger/logger.hpp>

#include <renderer/renderer.hpp>
#include <imgui/imgui.h>

bool CAimbot::IsEnabled() {
    if (!g_Vars.m_EnableAimbot || !CEngineClient::Get()->IsInGame()) return false;
    if (!CGlobal::Get().vdata || !CGlobal::Get().vdata->IsGun()) return false;
    return true;
}

void CAimbot::Run(const TargetData_t& data) {
    if (!IsEnabled()) {
        Invalidate();
        return;
    }

    target = data.player;
    CMoveData& lastMove = *CCSGOInput::Get()->moves.AtPtr(CCSGOInput::Get()->moves.m_Size - 1);
    const int weaponType = CGlobal::Get().vdata->m_WeaponType();
    const bool wantsRecoil = CGlobal::Get().pawn->m_iShotsFired() > 1 && weaponType != WEAPONTYPE_SHOTGUN &&
                             weaponType != WEAPONTYPE_SNIPER_RIFLE && !CGlobal::Get().weapon->m_bBurstMode();

    perfectAngle = lastMove.viewAngles;

    if (wantsRecoil) {
        lastMove.viewAngles.x -= CGlobal::Get().punchDelta.x * 2.f * g_Vars.m_RecoilX;
        lastMove.viewAngles.y -= CGlobal::Get().punchDelta.y * 2.f * g_Vars.m_RecoilY;
        lastMove.viewAngles.NormalizeAngle();
    }

    curAngle = lastMove.viewAngles;
    curAngle.z = 0.f;

    const float inaccuracy = CGlobal::Get().weapon->GetInaccuracy();

    if (inaccuracy > 0.05f) {
        Invalidate();
        return;
    }

    const Vector end = CGlobal::Get().eyePos + CGlobal::Get().rcsAngles.ToVector().Normalized() * 4096.f;

    C_CSPlayerPawn* hitPawn = nullptr;
    if (CGlobal::Get().weapon->m_nNextPrimaryAttackTick() < CGlobal::Get().controller->m_nTickBase()) {
        GameTrace_t trace;
        if (CEngineTrace::Get()->TraceShape(CGlobal::Get().eyePos, end, CGlobal::Get().pawn, 0x1C1003, 4, &trace))
            if (trace.hitEntity && trace.hitEntity->IsPlayerPawn()) hitPawn = static_cast<C_CSPlayerPawn*>(trace.hitEntity);
    }

    if (g_Vars.m_EnableTriggerbot && !wantsRecoil && hitPawn && hitPawn->IsEnemy(CGlobal::Get().pawn)) {
        lastMove.Scroll(IN_ATTACK);
    }

    const bool inDuel = g_Vars.m_EnableInDuel && (target ? target->dot >= g_Vars.m_ReactionTreshold : false);

    if (target) {
        ImVec2 screenPos;
        if (CMath::Get().WorldToScreen(data.aimPos, screenPos)) {
            const ImVec2& screenSize = ImGui::GetIO().DisplaySize;
            const float width = target->GetBBox().m_Maxs.x - target->GetBBox().m_Mins.x;
            const ImVec2 screenDistance = screenPos - screenSize * 0.5f;
            CFitts fitts(std::max(width, FLT_EPSILON), std::hypotf(screenDistance.x, screenDistance.y));
            target->fitts = std::max(fitts.Compute(0.f, 0.2f), 0.f);
        }

        if (IsInSmoke(CGlobal::Get().eyePos, data.aimPos)) {
            target = nullptr;
        }

        if (target && target->visibleSince < 0.15f && !inDuel) target = nullptr;
    }

    if (const bool isSwitchingTargets = oldTarget && oldTarget != target; isSwitchingTargets)
        lastTargetSwitchTime = CGlobalVars::Get()->currentTime;

    oldTarget = target;

    if (const bool inputDown = (lastMove.buttonsHeld & IN_ATTACK || lastMove.buttonsHeld & IN_SECOND_ATTACK); inputDown || inDuel)
        lastActiveTime = CGlobalVars::Get()->currentTime;

    const bool isSwitching = lastTargetSwitchTime - CGlobalVars::Get()->currentTime <= 0.15f;
    const float mouseLength = (float)std::hypot(lastMove.mouseDx, lastMove.mouseDy);
    const Vector angle = CMath::Get().CalculateAngle(CGlobal::Get().eyePos, data.aimPos);
    const float fov = CMath::Get().Fov(CGlobal::Get().rcsAngles, angle);
    shouldAim = target ? fov <= g_Vars.m_AimFov && CGlobalVars::Get()->currentTime - lastActiveTime <= 0.2f : false;

    if (target && shouldAim) {
        if (hitPawn && fov <= g_Vars.m_DelayFov && hitPawn != target->Get()->m_hPawn().Get()) {
            // prevent shoot
        }

        perfectAngle = angle - CGlobal::Get().aimPunch * 2.f;
        perfectAngle.NormalizeAngle();

        const Vector smoothedAngle = curAngle + Smooth(CGlobal::Get().rcsAngles, wantsRecoil ? perfectAngle : angle);
        const Vector curDelta = (perfectAngle - lastMove.viewAngles).NormalizedAngle();
        const Vector smoothedDelta = (perfectAngle - smoothedAngle).NormalizedAngle();

        // only apply the angle if it's beneficial
        if (fabsf(curDelta.x) > fabsf(smoothedDelta.x)) lastMove.viewAngles.x = smoothedAngle.x;

        if (fabsf(curDelta.y) > fabsf(smoothedDelta.y)) lastMove.viewAngles.y = smoothedAngle.y;
    } else {
        Invalidate();
    }

    lastMove.viewAngles.NormalizeAngle();
}

void CAimbot::Update() {
    // targetScreen = ImVec2(0, 0);
    // if (!target) return;
    // CMath::Get().WorldToScreen(targetPos, targetScreen);
}

void CAimbot::Render() {
    if (!CEngineClient::Get()->IsInGame()) return;

    auto drawList = CRenderer::GetBackgroundDrawList();
    if (!drawList) return;

    const ImGuiIO& io = ImGui::GetIO();
    const ImVec2 screenSize = io.DisplaySize;
    const ImVec2 center = screenSize * 0.5f;

    auto GetRadius = [&screenSize](float degrees, float fov = 90.f) {
        float scale = std::tanf(CMath::Deg2Rad(degrees)) / std::tanf(CMath::Deg2Rad(fov * 0.5f));
        return scale * screenSize.x * 0.5f;
    };

    if (g_Vars.m_DrawFov) {
        float r = GetRadius(g_Vars.m_AimFov) * 0.75f;
        drawList->AddCircle(center, r, IM_COL32(255, 255, 255, 255));
        r = GetRadius(g_Vars.m_DelayFov) * 0.75f;
        drawList->AddCircle(center, r, IM_COL32(255, 255, 255, 255));
    }

    if (target) {
        // drawList->AddCircle(targetScreen, 5.f, IM_COL32(255, 255, 255, 255));
    }
}

void CAimbot::Test(CCSGOInputHistoryEntryPB* historyEntry) {
    if (!g_Vars.m_Backtrack) return;
    if (!target || target->records.empty()) return;

    CCSGOInputHistoryEntryPB* subtick = historyEntry;
    if (!subtick || !subtick->pViewCmd || !subtick->cl_interp || !subtick->sv_interp0 || !subtick->sv_interp1) return;

    const auto [simTime1, simTime2, fraction] = CLagComp::Get().GetOptimalSimtime();
    if (fraction == -1.f) return;

    constexpr float interval = 1.f / 64.f;

    INetworkGameClient* client = INetworkGameClient::Get();
    if (!client) return;

    CNetworkGameClient* network = client->GetNetworkClient();
    if (!network) return;

    float simTime = simTime1;
    //simTime += network->GetClientInterp();
    int tick = static_cast<int>((simTime / interval) + 0.5f);

    Tickfrac_t tf{tick, 1.f};
    InterpInfo_t cl, sv0, sv1;
    if (!target->Get()->m_hPawn().Get()->m_pGameSceneNode()->CalculateInterpInfos(&cl, &sv0, &sv1, &tf)) return;

    CLogger::Log("Attempted to backtrack {} to tick {} + {:.1f}", subtick->nPlayerTickCount, tick, 1.f);
    subtick->nRenderTickCount = cl.dstTick;

    subtick->cl_interp->srcTick = cl.srcTick;
    subtick->cl_interp->dstTick = cl.dstTick;
    subtick->cl_interp->fraction = cl.fraction;

    subtick->sv_interp0->srcTick = sv0.srcTick;
    subtick->sv_interp0->dstTick = sv0.dstTick;
    subtick->sv_interp0->fraction = sv0.fraction;

    subtick->sv_interp1->srcTick = sv1.srcTick;
    subtick->sv_interp1->dstTick = sv1.dstTick;
    subtick->sv_interp1->fraction = sv1.fraction;

    //subtick->cl_interp->srcTick = tick;
    //subtick->cl_interp->dstTick = tick + 1;
    //subtick->cl_interp->fraction = 1.f;

    //subtick->sv_interp0->srcTick = tick;
    //subtick->sv_interp0->dstTick = tick+1;
    //subtick->sv_interp0->fraction = 0.f;

    //subtick->sv_interp1->srcTick = tick+1;
    //subtick->sv_interp1->dstTick = tick+2;
    //subtick->sv_interp1->fraction = 0.f;
}

void CAimbot::Invalidate() {
    target = nullptr;
    pid[0].Reset();
    pid[1].Reset();
    targetScreen = ImVec2(0, 0);
    shouldAim = false;
}

bool CAimbot::IsInSmoke(const Vector& start, const Vector& end) {
    static auto func = signatures::LineGoesThroughSmoke.GetPtrAs<float (*)(const Vector&, const Vector&, uintptr_t)>();
    return func && func(start, end, 0) >= 1.0f;
}

Vector CAimbot::Smooth(const Vector& from, const Vector& to) {
    const float bonus = target->dot >= g_Vars.m_ReactionTreshold ? 1.f : g_Vars.m_Bonus + 1.f;  // [1.f - 2.f]
    const PIDConfig_t pidCfg{
        .m_KP = g_Vars.m_KP * 0.3f * bonus, .m_KI = g_Vars.m_KI * 0.3f * bonus, .m_kd = 0.f, .m_Damp = 1.f - g_Vars.m_Damp};

    Vector delta = (to - from).NormalizedAngle();
    delta.x = pid[0].Step(delta.x, 0.015f, pidCfg);
    delta.y = pid[1].Step(delta.y, 0.015f, pidCfg);
    return delta;
}
