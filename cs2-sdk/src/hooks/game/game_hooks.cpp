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
    CCachedPlayer* cachedLocal = CMatchCache::Get().GetLocalPlayer();
    if (!cachedLocal)
        return g_CreateMove.CallOriginal<void>(rcx, subtick, active);
    CCSPlayerController* localController = cachedLocal->Get();
    if (!localController)
        return g_CreateMove.CallOriginal<void>(rcx, subtick, active);
    C_CSPlayerPawn* localPawn = localController->m_hPawn().Get();
    if (!localPawn)
        return g_CreateMove.CallOriginal<void>(rcx, subtick, active);

    const bool grounded = localPawn->m_hGroundEntity().Get() != nullptr;

    #if 0
    // override the current data
    if (rcx->moveData.buttonsHeld & IN_ATTACK) rcx->moveData.buttonsHeld |= IN_JUMP;
    if (rcx->moveData.buttonsScroll & IN_ATTACK) rcx->moveData.buttonsScroll |= IN_JUMP;
    if (rcx->moveData.prevButtonsHeld & IN_ATTACK) rcx->moveData.prevButtonsHeld |= IN_JUMP;
    if (rcx->moveData.buttonsChanged & IN_ATTACK) rcx->moveData.buttonsChanged |= IN_JUMP;
    #endif



    // override the last subtick info
    if (rcx->subtickMoves.m_Size) {
        CMoveData* move = rcx->subtickMoves.AtPtr(rcx->subtickMoves.m_Size - 1);

        //if (grounded) move->buttonsHeld |= IN_JUMP;
        //else if (move->prevButtonsHeld & IN_JUMP) move->prevButtonsHeld &= ~IN_JUMP;
        //if (grounded) move->buttonsScroll |= IN_JUMP;
        //else if (move->prevButtonsHeld & IN_JUMP) move->prevButtonsHeld &= ~IN_JUMP;

        for (uint32_t j = 0; j < move->subtickCount; ++j) {
            CSubtickMove& subtickMove = move->subtickMoves[j];
            CLogger::Log("{} subtickMove {}: {} [{}, {}, {}]", rcx->sequenceNumber, j, subtickMove.when, subtickMove.lastPressedButtons, subtickMove.analog_forward_delta,
                						 subtickMove.analog_sidemove_delta);
            //if (grounded) subtickMove.lastPressedButtons |= IN_JUMP;
            //else if (subtickMove.lastPressedButtons & IN_JUMP) subtickMove.lastPressedButtons &= ~IN_JUMP;
        }

        if (grounded && rcx->moveData.buttonsHeld & IN_JUMP) {
            for (uint32_t j = 0; j < move->subtickCount; ++j) {
                CSubtickMove& subtickMove = move->subtickMoves[j];
                subtickMove.lastPressedButtons &= ~IN_JUMP;
            }


            // we have enough subticks to input a jump
            if (move->subtickCount > 2) {
                CSubtickMove& subtickMove = move->subtickMoves[move->subtickCount - 2];
				subtickMove.lastPressedButtons |= IN_JUMP;
                subtickMove.analog_forward_delta = 1;
                subtickMove = move->subtickMoves[move->subtickCount - 1];
                subtickMove.lastPressedButtons |= IN_JUMP;
                subtickMove.analog_forward_delta = 0;
            } else {
                // we don't have enough subticks to input a jump
                const uint32_t tick = (move->subtickCount += 2) - 1;
                move->subtickMoves[tick] = CSubtickMove{
                    .when = 0.92f,
                    .lastPressedButtons = IN_JUMP,
                    .analog_forward_delta = 1,
                    .analog_sidemove_delta = 0,
                };
                move->subtickMoves[tick - 1] = move->subtickMoves[tick];
                move->subtickMoves[tick - 1].analog_forward_delta = 0;
            }
        }
    }

    g_CreateMove.CallOriginal<bool>(rcx, subtick, active);
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
