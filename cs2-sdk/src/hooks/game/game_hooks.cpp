#include "pch.hpp"

#include <intrin.h>

#include <hooks/game/game_hooks.hpp>
#include <logger/logger.hpp>

#include <signatures/signatures.hpp>
#include <constants/constants.hpp>

#include <hacks/esp/esp.hpp>
#include <hacks/chams/chams.hpp>
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
#include <interfaces/materialsystem.hpp>

#include <hacks/aimbot/aimbot.hpp>
#include <hacks/visuals/visuals.hpp>
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

    if (!CEngineClient::Get()->IsInGame()) return;

    if (auto local = CMatchCache::Get().GetLocalPlayer(); local && local->IsValid()) {
        if (auto pawn = local->Get()->m_hPawn().Get(); !pawn->m_bIsScoped()) {
             view->fov += g_Vars.m_Fov;
             view->viewmodelFov += g_Vars.m_ViewmodelFov;
        }
    }

    CMath::Get().UpdateViewMatrix(pWorldToProjection);

    // SDKTODO: May sometimes stutter (?)
    //  Find a better function that won't microstutter,
    //  should also consider "host_timescale" factor.
    CESP::Get().Update();
}

static CHook g_CreateMove;
static void hkCreateMove(CCSGOInput* rcx, int subtick, char active) {
    if (!CEngineClient::Get()->IsInGame()) return g_CreateMove.CallOriginal<void>(rcx, subtick, active);
    if (rcx->moves.m_Size == 0) return g_CreateMove.CallOriginal<void>(rcx, subtick, active);
    CCachedPlayer* cachedLocal = CMatchCache::Get().GetLocalPlayer();
    if (!cachedLocal) return g_CreateMove.CallOriginal<void>(rcx, subtick, active);
    CCSPlayerController* localController = cachedLocal->Get();
    if (!localController) return g_CreateMove.CallOriginal<void>(rcx, subtick, active);
    C_CSPlayerPawn* localPawn = localController->m_hPawn().Get();
    if (!localPawn) return g_CreateMove.CallOriginal<void>(rcx, subtick, active);

    CMoveData* move = rcx->moves.AtPtr(rcx->moves.m_Size - 1);

    // if (CInputHandler::Get().IsHeld(IN_JUMP)) CLogger::Log("Jump down");
    // if (CInputHandler::Get().IsPressed(IN_JUMP)) CLogger::Log("Jump pressed");
    // if (CInputHandler::Get().IsReleased(IN_JUMP)) CLogger::Log("Jump released");

    CInputHandler::Get().Poll(rcx);

    const bool grounded = localPawn->m_hGroundEntity().Get() != nullptr;

    // override the last subtick info

    if (CUserCmd* cmd = rcx->GetUserCmd(); cmd && cmd->vftable) {
        if (CBaseUserCmdPB* baseCmd = cmd->csgoUserCmd.baseCmd; baseCmd && baseCmd->subticksMoveSteps.currentSize > 0) {
            for (int i = 0; i < baseCmd->subticksMoveSteps.currentSize; ++i) {
                if (CSubtickMoveStep* step = baseCmd->subticksMoveSteps.rep->elements[i]; step)
                    CLogger::Log("subtick: {} {:#x} {} {}", i, step->button, step->when, step->pressed);
            }
        }
    }

    CAimbot::Get().Run(move);

    #if 0
    static bool shouldInputJump = false;
    if (!grounded) {
        shouldInputJump = CInputHandler::Get().IsReleased(IN_JUMP) || shouldInputJump;
    } else {
        if (CInputHandler::Get().IsHeld(IN_JUMP)) {
            CInputHandler::Get().Press(IN_JUMP, 0.9f);
            CLogger::Log("Inputting jump at subtick {}", move->subtickCount - 1);
            shouldInputJump = false;
        }
    }
    #endif

    // CInputHandler::Get().Dump();


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
    CVisuals::Get().OnFrameStageNotify(stage);
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

//static CHook g_SetButtonStates;
//static void hkSetButtonStates(ButtonState_t* state, uint64_t button, uint8_t flags) { 
//  uint64_t forceHeld = 0, forceChanged = 0, forceScroll = 0;
//  if (flags & 1) forceHeld = button;
//  if (flags & 2) forceChanged = button;
//  if (flags & 4) forceScroll = button;
//
//  if (false && button & IN_JUMP) {
//      CLogger::Log("Held: {}, Changed: {}, Scroll: {}", forceHeld, forceChanged, forceScroll);
//  }
//
//  state->value = state->value & ~button | forceHeld;
//  state->valueChanged = state->valueChanged & ~button | forceChanged;
//  state->valueScroll = state->valueScroll & ~button | forceScroll;
//
//  if (false && button & IN_JUMP) {
//      CLogger::Log("input| Held: {}, Changed: {}, Scroll: {} prev:{}", CCSGOInput::Get()->moveData.buttonsHeld,
//                   CCSGOInput::Get()->moveData.buttonsChanged, CCSGOInput::Get()->moveData.buttonsScroll,
//                   CCSGOInput::Get()->moveData.prevButtonsHeld);
//      CLogger::Log("Held: {}, Changed: {}, Scroll: {}", state->value, state->valueChanged, state->valueScroll);
//      CLogger::Log("=====================================================");
//  }
//}

static CHook g_MoveData;
static void hkMoveData(CCSGOInput* rcx, int slot) { 
  auto c = rcx->moves.m_Size;
  for ( int i = 0; i < c; i++ )
  {
    CMoveData* move = rcx->moves.AtPtr(i);
      if (move && move->subtickCount)
    {
      CLogger::Log("pre subtick: {} {:#x} {:#x} {:#x}", move->subtickCount, move->buttonsHeld, move->buttonsChanged, move->buttonsScroll);

    }
  }

  g_MoveData.CallOriginal<void>(rcx, slot); 

  c = rcx->moves.m_Size;
  for (int i = 0; i < c; i++) {
      CMoveData* move = rcx->moves.AtPtr(i);
      if (move && move->subtickCount) {
          CLogger::Log("post subtick: {} {:#x} {:#x} {:#x}", move->subtickCount, move->buttonsHeld, move->buttonsChanged, move->buttonsScroll);
      }
  }  

}

static CHook g_DrawObject;
static void hkDrawObject(void* animatableSceneObjectDesc, void* dx11, CMeshData* meshDraw, int dataCount, void* sceneView, void* sceneLayer, void* unk, void* unk2) {
    if (!CEngineClient::Get()->IsInGame())
      return g_DrawObject.CallOriginal<void>(animatableSceneObjectDesc, dx11, meshDraw, dataCount, sceneView, sceneLayer, unk, unk2);

    if (!CChams::Get().OnDrawObject(animatableSceneObjectDesc, dx11, meshDraw, dataCount, sceneView, sceneLayer, unk, unk2)) 
      return g_DrawObject.CallOriginal<void>(animatableSceneObjectDesc, dx11, meshDraw, dataCount, sceneView, sceneLayer, unk, unk2);

    g_DrawObject.CallOriginal<void>(animatableSceneObjectDesc, dx11, meshDraw, dataCount, sceneView, sceneLayer, unk, unk2);
}

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
    //g_SetButtonStates.Hook(signatures::SetButtonStates.GetPtrAs<void*>(), SDK_HOOK(hkSetButtonStates));
    //g_MoveData.Hook(signatures::MoveData.GetPtrAs<void*>(), SDK_HOOK(hkMoveData));
    g_DrawObject.Hook(signatures::DrawObject.GetPtrAs<void*>(), SDK_HOOK(hkDrawObject));
}
