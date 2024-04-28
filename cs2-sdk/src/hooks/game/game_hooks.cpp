#include "pch.hpp"

#include <intrin.h>

#include <hooks/game/game_hooks.hpp>
#include <logger/logger.hpp>

#include <signatures/signatures.hpp>
#include <constants/constants.hpp>

#include <hacks/esp/esp.hpp>
#include <cache/cache.hpp>
#include <menu/menu.hpp>
#include <math/math.hpp>
#include <hook/hook.hpp>
#include <input/input.hpp>

#include <cache/entities/player.hpp>
#include <bindings/playercontroller.hpp>
#include <bindings/playerpawn.hpp>
#include <bindings/weapon.hpp>

#include <interfaces/gameentitysystem.hpp>
#include <interfaces/source2client.hpp>
#include <interfaces/engineclient.hpp>
#include <interfaces/enginetrace.hpp>
#include <interfaces/ccsgoinput.hpp>
#include <interfaces/inventory.hpp>
#include <interfaces/viewrender.hpp>

#include <hacks/aimbot/aimbot.hpp>
#include <hacks/skinchanger/skinchanger.hpp>

#include <vars/vars.hpp>

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

static void hkGetMatricesForView(void* rcx, CViewSetup* view, VMatrix* pWorldToView, VMatrix* pViewToProjection,
                                 VMatrix* pWorldToProjection, VMatrix* pWorldToPixels) {
    g_GetMatricesForView.CallOriginal<void>(rcx, view, pWorldToView, pViewToProjection, pWorldToProjection, pWorldToPixels);

    // view->fov += g_Vars.m_Fov;
    // view->viewmodelFov += g_Vars.m_ViewmodelFov;

    CMath::Get().UpdateViewMatrix(pWorldToProjection);

    // SDKTODO: May sometimes stutter (?)
    //  Find a better function that won't microstutter,
    //  should also consider "host_timescale" factor.
    CESP::Get().Update();
}

static CHook g_CreateMove;
static void hkCreateMove(CCSGOInput* rcx, int subtick, char active) {
    CInputHandler::Get().Poll(rcx);

    if (rcx->subtickMoves.m_Size == 0) return g_CreateMove.CallOriginal<void>(rcx, subtick, active);
    CCachedPlayer* cachedLocal = CMatchCache::Get().GetLocalPlayer();
    if (!cachedLocal) return g_CreateMove.CallOriginal<void>(rcx, subtick, active);
    CCSPlayerController* localController = cachedLocal->Get();
    if (!localController) return g_CreateMove.CallOriginal<void>(rcx, subtick, active);
    C_CSPlayerPawn* localPawn = localController->m_hPawn().Get();
    if (!localPawn) return g_CreateMove.CallOriginal<void>(rcx, subtick, active);

    //if (CInputHandler::Get().IsHeld(IN_JUMP)) CLogger::Log("Jump down");
    //if (CInputHandler::Get().IsPressed(IN_JUMP)) CLogger::Log("Jump pressed");
    //if (CInputHandler::Get().IsReleased(IN_JUMP)) CLogger::Log("Jump released");

    const bool grounded = localPawn->m_hGroundEntity().Get() != nullptr;
    const int lastIndex = rcx->subtickMoves.m_Size - 1;

    // override the last subtick info

    CMoveData* move = rcx->subtickMoves.AtPtr(lastIndex);
    CAimbot::Get().Run(move);

    #if 0
    static bool shouldInputJump = false;
    if (!grounded) {
        shouldInputJump = CInputHandler::Get().Release(IN_JUMP) || shouldInputJump;
    } else {
        if ((CInputHandler::Get().IsHeld(IN_JUMP) || CInputHandler::Get().IsHeld(IN_ATTACK2)) && shouldInputJump) {
            CInputHandler::Get().Press(IN_JUMP, 0.9f);
            CLogger::Log("Inputting jump at subtick {}", move->subtickCount - 1);
            shouldInputJump = false;
        }
    }
    #endif

    // static auto GetBool = signatures::GetBool.GetPtrAs<bool* (*)(uintptr_t, int)>();
    // if (GetBool) {
    //     uintptr_t grenadeCvar = signatures::GrenadePtr.GetPtr().Get();
    //     bool* grenadePreview = GetBool(grenadeCvar, -1);
    //     if ( grenadePreview )
    //     {
    //         *grenadePreview = true;
    //         CLogger::Log("Grenade preview enabled");
    //     }
    // }

    g_CreateMove.CallOriginal<bool>(rcx, subtick, active);
}

static CHook g_CreateMove2;
static bool hkCreateMove2(CCSGOInput* rcx, int subtick, void* a3) {
    bool ret = g_CreateMove2.CallOriginal<bool>(rcx, subtick, a3);

    return ret;
}

static CHook g_FrameStageNotify;
static void hkFrameStageNotify(void* rcx, int stage) {
    CSkinChanger::Get().OnFrameStageNotify(stage);
    g_FrameStageNotify.CallOriginal<void>(rcx, stage);
}

static CHook g_FireEventClientSide;
static bool hkFireEventClientSide(void* rcx, CGameEvent* event, bool serverOnly) {
    CSkinChanger::Get().OnPreFireEvent(event);
    return g_FireEventClientSide.CallOriginal<bool>(rcx, event, serverOnly);
}

static CHook g_EquipItemInLoadout;
static bool hkEquipItemInLoadout(void* rcx, int item, int slot, uint64_t itemID) {
    CSkinChanger::Get().OnEquipItemInLoadout(item, slot, itemID);
    return g_EquipItemInLoadout.CallOriginal<bool>(rcx, item, slot, itemID);
}

static CHook g_SetModel;
static void* hkSetModel(void* rcx, const char* model) {
    CSkinChanger::Get().OnSetModel(static_cast<C_BaseModelEntity*>(rcx), model);
    return g_SetModel.CallOriginal<void*>(rcx, model);
}

static CHook g_IsLoadoutAllowed;
static bool hkIsLoadoutAllowed() { return true; }

void CGameHooks::Initialize() {
    SDK_LOG_PROLOGUE();

    CMatchCache::Get().Initialize();

    g_MouseInputEnabled.VHook(CCSGOInput::Get(), platform::Constant(13, 14), SDK_HOOK(hkMouseInputEnabled));
    g_CreateMove.VHook(CCSGOInput::Get(), platform::Constant(5, 5), SDK_HOOK(hkCreateMove));
    // g_CreateMove2.VHook(CCSGOInput::Get(), platform::Constant(15, 15), SDK_HOOK(hkCreateMove2));
    g_OnAddEntity.VHook(CGameEntitySystem::Get(), platform::Constant(14, 15), SDK_HOOK(hkOnAddEntity));
    g_OnRemoveEntity.VHook(CGameEntitySystem::Get(), platform::Constant(15, 16), SDK_HOOK(hkOnRemoveEntity));
    g_FrameStageNotify.VHook(CSource2Client::Get(), platform::Constant(35, 36), SDK_HOOK(hkFrameStageNotify));
    g_EquipItemInLoadout.VHook(CCSInventoryManager::Get(), platform::Constant(52, 53), SDK_HOOK(hkEquipItemInLoadout));
    g_GetMatricesForView.Hook(signatures::GetMatricesForView.GetPtrAs<void*>(), SDK_HOOK(hkGetMatricesForView));
    g_FireEventClientSide.Hook(signatures::FireEventClientSide.GetPtrAs<void*>(), SDK_HOOK(hkFireEventClientSide));
    g_SetModel.Hook(signatures::SetModel.GetPtrAs<void*>(), SDK_HOOK(hkSetModel));
    g_IsLoadoutAllowed.Hook(signatures::IsLoadoutAllowed.GetPtrAs<void*>(), SDK_HOOK(hkIsLoadoutAllowed));
}
