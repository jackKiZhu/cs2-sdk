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
#include <interfaces/ccsgoinput.hpp>

#include <hacks/aimbot/aimbot.hpp>

static CHook g_MouseInputEnabled;

static bool hkMouseInputEnabled(void* rcx) {
    return CMenu::Get().IsOpen() ? false : g_MouseInputEnabled.CallOriginal<bool>(rcx);
}

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

static void hkCreateMove(CCSGOInput* rcx, int subtick, char active) {
    /*if (rcx->moveData.buttonsHeld & IN_ATTACK)
        rcx->moveData.buttonsHeld |= IN_JUMP;
    if (rcx->moveData.buttonsScroll & IN_ATTACK)
		rcx->moveData.buttonsScroll |= IN_JUMP;
    if (rcx->moveData.prevButtonsHeld & IN_ATTACK)
        rcx->moveData.prevButtonsHeld |= IN_JUMP;
    if (rcx->moveData.buttonsChanged & IN_ATTACK) 
        rcx->moveData.buttonsChanged |= IN_JUMP;*/

    if (rcx->sub_tick_moves->buttonsHeld & IN_ATTACK)
        rcx->sub_tick_moves->buttonsHeld |= IN_JUMP;
    if (rcx->sub_tick_moves->prevButtonsHeld & IN_ATTACK)
        rcx->sub_tick_moves->prevButtonsHeld |= IN_JUMP;
    /*
    if (rcx->sub_tick_moves->buttonsScroll & IN_ATTACK) rcx->sub_tick_moves->buttonsScroll |= IN_JUMP;
    if (rcx->sub_tick_moves->buttonsChanged & IN_ATTACK) rcx->sub_tick_moves->buttonsChanged |= IN_JUMP;
    */

    /*for (uint32_t i = 0; i < rcx->moveData.subtickCount; ++i) {
        CSubtickMove move = rcx->moveData.subtickMoves[i];
        if (move.lastPressedButtons & IN_ATTACK) 
            move.lastPressedButtons |= IN_JUMP;
    }*/

    for (uint32_t i = 0; i < rcx->total_subtick_data; ++i) {
        auto subtickData = &rcx->sub_tick_moves[i];
        for (auto j = 0; j < subtickData->subtickCount; ++j) {
            auto& move = subtickData->subtickMoves[j];
            if (move.lastPressedButtons & IN_ATTACK)
                move.lastPressedButtons |= IN_JUMP;
        }
    }

    // setup subticks moves here
    //if (subtick) {
    //    // this is overriden in CClientInput::UpdateInputs
    //    if (true || rcx->moveData.buttonsChanged & IN_ATTACK) {
    //        rcx->moveData.buttonsHeld |= IN_JUMP;
    //        rcx->moveData.buttonsScroll |= IN_JUMP;
    //        for (uint32_t i = 0; i < rcx->moveData.subtickCount; ++i) {
    //            CSubtickMove move = rcx->moveData.subtickMoves[i];
    //            move.lastPressedButtons = rcx->moveData.buttonsHeld;
    //            *(uint32_t*)(&move.analog_forward_delta) = 1;
    //        }
    //    }
    //}

    if (rcx->moveData.buttonsChanged || rcx->moveData.buttonsHeld || rcx->moveData.buttonsScroll || rcx->moveData.prevButtonsHeld) {
        CLogger::Log("PreCreateMove5: {}:{} [{}, {}, {}, {}]", rcx->sequenceNumber, subtick, rcx->moveData.buttonsChanged,
                     rcx->moveData.buttonsHeld, rcx->moveData.buttonsScroll, rcx->moveData.prevButtonsHeld);
    }

    g_CreateMove.CallOriginal<bool>(rcx, subtick, active);
    if (rcx->sub_tick_moves->buttonsHeld & IN_ATTACK)
        rcx->sub_tick_moves->buttonsHeld |= IN_JUMP;
    if (rcx->moveData.buttonsChanged || rcx->moveData.buttonsHeld || rcx->moveData.buttonsScroll || rcx->moveData.prevButtonsHeld) {
        CLogger::Log("PostCreateMove5: {}:{} [{}, {}, {}, {}]\n", rcx->sequenceNumber, subtick, rcx->moveData.buttonsChanged,
                     rcx->moveData.buttonsHeld,
                     rcx->moveData.buttonsScroll, rcx->moveData.prevButtonsHeld);
    }

    /*
[cs2-sdk] CreateMove15: 13259 [0, 0, 0, 0]
[cs2-sdk] CreateMove5: 13259 [0, 0, 0, 0]
[cs2-sdk] SetViewAngles: 13259 [0, 0, 0, 0]

[cs2-sdk] CreateMove15: 13260 [1, 1, 0, 0]
[cs2-sdk] CreateMove5: 13260 [1, 1, 0, 0]
[cs2-sdk] CreateMove15: 13260 [1, 1, 0, 0]
[cs2-sdk] CreateMove5: 13260 [1, 1, 0, 0]
[cs2-sdk] CreateMove15: 13260 [1, 1, 0, 0]
[cs2-sdk] CreateMove5: 13260 [1, 1, 0, 0]
[cs2-sdk] CreateMove15: 13260 [1, 1, 0, 0]
[cs2-sdk] CreateMove5: 13260 [1, 1, 0, 0]
[cs2-sdk] CreateMove15: 13260 [1, 1, 0, 0]
[cs2-sdk] CreateMove5: 13260 [1, 1, 0, 0]
[cs2-sdk] CreateMove15: 13260 [1, 1, 0, 0]
[cs2-sdk] CreateMove5: 13260 [1, 1, 0, 0]
[cs2-sdk] SetViewAngles: 13260 [0, 1, 0, 1]

[cs2-sdk] CreateMove15: 13261 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13261 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13261 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13261 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13261 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13261 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13261 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13261 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13261 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13261 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13261 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13261 [0, 1, 0, 1]
[cs2-sdk] SetViewAngles: 13261 [0, 1, 0, 1]

[cs2-sdk] CreateMove15: 13262 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13262 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13262 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13262 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13262 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13262 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13262 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13262 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13262 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13262 [0, 1, 0, 1]
[cs2-sdk] SetViewAngles: 13262 [0, 1, 0, 1]

[cs2-sdk] CreateMove15: 13263 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13263 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13263 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13263 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13263 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13263 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13263 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13263 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13263 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13263 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13263 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13263 [0, 1, 0, 1]
[cs2-sdk] SetViewAngles: 13263 [0, 1, 0, 1]

[cs2-sdk] CreateMove15: 13264 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13264 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13264 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13264 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13264 [0, 1, 0, 1]
[cs2-sdk] CreateMove5: 13264 [0, 1, 0, 1]
[cs2-sdk] CreateMove15: 13264 [0, 0, 1, 1]
[cs2-sdk] CreateMove5: 13264 [0, 0, 1, 1]
[cs2-sdk] CreateMove15: 13264 [0, 0, 1, 1]
[cs2-sdk] CreateMove5: 13264 [0, 0, 1, 1]
[cs2-sdk] CreateMove15: 13264 [0, 0, 1, 1]
[cs2-sdk] CreateMove5: 13264 [0, 0, 1, 1]
[cs2-sdk] SetViewAngles: 13264 [0, 0, 0, 0]

[cs2-sdk] CreateMove15: 13265 [0, 0, 0, 0]
    */

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

static CHook g_CreateMove2;

static bool hkCreateMove2(CCSGOInput* rcx, int subtick, void* a3) {
    bool ret = g_CreateMove2.CallOriginal<bool>(rcx, subtick, a3);

    return ret;
}

static CHook g_SetViewAngles;

static void hkSetViewAngles(CCSGOInput* rcx, int subtick) {
    g_SetViewAngles.CallOriginal<void>(rcx, subtick);

    //CLogger::Log("SetViewAngles: {} [{}, {}, {}, {}]", rcx->sequenceNumber, rcx->moveData.buttonsChanged, rcx->moveData.buttonsHeld,
    //             rcx->moveData.buttonsScroll, rcx->moveData.prevButtonsHeld);

    CAimbot::Get().Run();
}

void CGameHooks::Initialize() {
    SDK_LOG_PROLOGUE();

    CMatchCache::Get().Initialize();

    g_MouseInputEnabled.VHook(CCSGOInput::Get(), platform::Constant(13, 14), SDK_HOOK(hkMouseInputEnabled));
    g_CreateMove.VHook(CCSGOInput::Get(), platform::Constant(5, 5), SDK_HOOK(hkCreateMove));
    g_CreateMove2.VHook(CCSGOInput::Get(), platform::Constant(15, 15), SDK_HOOK(hkCreateMove2));
    g_SetViewAngles.VHook(CCSGOInput::Get(), platform::Constant(7, 7), SDK_HOOK(hkSetViewAngles));
    g_OnAddEntity.VHook(CGameEntitySystem::Get(), platform::Constant(14, 15), SDK_HOOK(hkOnAddEntity));
    g_OnRemoveEntity.VHook(CGameEntitySystem::Get(), platform::Constant(15, 16), SDK_HOOK(hkOnRemoveEntity));
    g_GetMatricesForView.Hook(signatures::GetMatricesForView.GetPtrAs<void*>(), SDK_HOOK(hkGetMatricesForView));
}
