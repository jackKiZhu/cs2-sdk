#include "pch.hpp"

#include <hooks/game/game_hooks.hpp>
#include <logger/logger.hpp>

#include <signatures/signatures.hpp>
#include <constants/constants.hpp>

#include <hacks/esp/esp.hpp>
#include <cache/cache.hpp>
#include <menu/menu.hpp>
#include <math/math.hpp>
#include <hook/hook.hpp>

#include <cache/entities/player.hpp>
#include <bindings/playercontroller.hpp>
#include <bindings/playerpawn.hpp>
#include <bindings/weapon.hpp>

#include <interfaces/gameentitysystem.hpp>
#include <interfaces/source2client.hpp>
#include <interfaces/engineclient.hpp>
#include <interfaces/enginetrace.hpp>
#include <input/ccsgoinput.hpp>

#include <hacks/aimbot/aimbot.hpp>

static CHook g_MouseInputEnabled;
static bool hkMouseInputEnabled(void* rcx) { return CMenu::Get().IsOpen() ? false : g_MouseInputEnabled.CallOriginal<bool>(rcx); }

static CHook g_OnAddEntity;
static void* hkOnAddEntity(void* rcx, CEntityInstance* inst, CBaseHandle handle) {
    CMatchCache::Get().AddEntity(inst, handle);
    return g_OnAddEntity.CallOriginal<void*>(rcx, inst, handle);
}

static CHook g_OnRemoveEntity;
static void* hkOnRemoveEntity(void* rcx, CEntityInstance* inst, CBaseHandle handle) {
    CMatchCache::Get().RemoveEntity(inst, handle);
    return g_OnRemoveEntity.CallOriginal<void*>(rcx, inst, handle);
}

static CHook g_GetMatricesForView;
static void hkGetMatricesForView(void* rcx, void* view, VMatrix* pWorldToView, VMatrix* pViewToProjection, VMatrix* pWorldToProjection,
                                 VMatrix* pWorldToPixels) {
    g_GetMatricesForView.CallOriginal<void>(rcx, view, pWorldToView, pViewToProjection, pWorldToProjection, pWorldToPixels);

    CMath::Get().UpdateViewMatrix(pWorldToProjection);

    // SDKTODO: May sometimes stutter (?)
    //  Find a better function that won't microstutter,
    //  should also consider "host_timescale" factor.
    CESP::Get().Update();
}

static CHook g_CreateMove;
static void hkCreateMove(CCSGOInput* rcx, int slot, char active) {

    //CLogger::Log("Viewangles: {} {} {}", rcx->viewAngles.x, rcx->viewAngles.y, rcx->viewAngles.z);

    //rcx->viewAngles.y += 1.f;
    //rcx->viewAngles.NormalizeAngle();

    g_CreateMove.CallOriginal<bool>(rcx, slot, active);

    #if 0

    static auto GetSequenceNumber = signatures::GetSequenceNumber.GetPtrAs<int (*)(void*, int)>();
    if (!GetSequenceNumber) return;

    static auto GetUserCmd = signatures::GetUserCmd.GetPtrAs<CUserCmd* (*)(void*, int, int, bool)>();
    if (!GetUserCmd) return;

    if (!CEngineClient::Get()->IsInGame()) return;

    int sequenceNumber = GetSequenceNumber(nullptr, 0);
    if (sequenceNumber < 0 || slot) return;

    CUserCmd* cmd = GetUserCmd(rcx, 0, sequenceNumber, true);
    //CUserCmd* cmd = rcx->GetUserCmd()
    if (!cmd) return;

    CBaseUserCmdPB* baseCmd = cmd->baseCmd;
    if (!baseCmd) return;

    CMsgQAngle* msgAngle = baseCmd->msgAngle;
    if (!msgAngle) return;

    CCachedPlayer* cachedLocal = CMatchCache::Get().GetLocalPlayer();
    if (!cachedLocal) return;

    CCSPlayerController* controller = cachedLocal->Get();
    if (!controller) return;

    C_CSPlayerPawnBase* pawn = controller->m_hPawn().Get();
    if (!pawn) return;
    #endif
}

static CHook g_SetViewAngles;
static void hkSetViewAngles(CCSGOInput* rcx, int subtick) {
    g_SetViewAngles.CallOriginal<void>(rcx, subtick);

    CAimbot::Get().Run(rcx);
}

void CGameHooks::Initialize() {
    SDK_LOG_PROLOGUE();

    CMatchCache::Get().Initialize();

    g_MouseInputEnabled.VHook(CCSGOInput::Get(), platform::Constant(13, 14), SDK_HOOK(hkMouseInputEnabled));
    g_CreateMove.VHook(CCSGOInput::Get(), platform::Constant(5, 5), SDK_HOOK(hkCreateMove));
    g_SetViewAngles.VHook(CCSGOInput::Get(), platform::Constant(7, 7), SDK_HOOK(hkSetViewAngles));
    g_OnAddEntity.VHook(CGameEntitySystem::Get(), platform::Constant(14, 15), SDK_HOOK(hkOnAddEntity));
    g_OnRemoveEntity.VHook(CGameEntitySystem::Get(), platform::Constant(15, 16), SDK_HOOK(hkOnRemoveEntity));
    g_GetMatricesForView.Hook(signatures::GetMatricesForView.GetPtrAs<void*>(), SDK_HOOK(hkGetMatricesForView));
}
