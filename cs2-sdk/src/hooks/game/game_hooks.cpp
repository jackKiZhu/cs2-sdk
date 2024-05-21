#include <pch.hpp>

#include <intrin.h>

#include <hooks/game/game_hooks.hpp>
#include <logger/logger.hpp>

#include <signatures/signatures.hpp>
#include <constants/constants.hpp>

#include <hacks/global/global.hpp>
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

#include <types/utlbuffer.hpp>

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
    CUserCmd* cmd = rcx->GetUserCmd();
    if (!CGlobal::Get().Update(cmd)) return g_CreateMove.CallOriginal<void>(rcx, subtick, active);

    const bool grounded = CGlobal::Get().pawn->m_hGroundEntity().Get() != nullptr;

    CLagComp::Get().Update();

    auto targetData = CLagComp::Get().Find();
    CAimbot::Get().Run(targetData);

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

    //CLogger::Log("Pre checksum: {}", (uintptr_t)cmd->csgoUserCmd.baseCmd->moveCRC);

    g_CreateMove.CallOriginal<bool>(rcx, subtick, active);

    CAimbot::Get().Backtrack(targetData);

    //cmd->csgoUserCmd.baseCmd->buttons->scroll |= IN_JUMP;  
    //cmd->GetBaseCmdButtons();

    #if 0
    static auto SerializePartialToArray = signatures::SerializePartialToArray.GetPtrAs<bool (*)(void*, CUtlBuffer*, int)>();
    static auto CalculateCRC = signatures::CalculateCRC.GetPtrAs<void(*)(void*, const void*, size_t)>();
    static auto SetCRC = signatures::SetCRC.GetPtrAs<void (*)(void*, void*, void*)>();

    auto buffer = CUtlBuffer();
    const int CRCSize = cmd->csgoUserCmd.baseCmd->Checksum();
    buffer.EnsureCapacity(CRCSize + 1);
    if (!SerializePartialToArray(cmd->csgoUserCmd.baseCmd, &buffer, CRCSize)) {
        CLogger::Log("Failed to serialize partial to array");
        return;
    }

    uint64_t crc[4];
    memset(crc, 0, sizeof(crc));
    CalculateCRC(crc, buffer.Base(), CRCSize);

    uint64_t* v95 = (uint64_t*)(*(uint64_t*)&cmd->csgoUserCmd.hasBits & 0xFFFFFFFFFFFFFFFC);
    if ( cmd->csgoUserCmd.hasBits & 1)
      v95 = (uint64_t*)*v95;
    SetCRC(cmd->csgoUserCmd.baseCmd, crc, v95);

    CLogger::Log("Post checksum: {}\n", (uintptr_t)cmd->csgoUserCmd.baseCmd->moveCRC);
    #endif

    const CInButtonState originalButtons = cmd->buttons;

    // after this, all the subticks have been processed into the protobufs messages
    // it is better to change the message _before_ calling original but for that you need to meddle with CMoveData and i can't bother.

    // cmd->buttons.changed |= IN_JUMP;


    #if 0
    if (CBaseUserCmdPB* baseCmd = cmd->csgoUserCmd.baseCmd; baseCmd && baseCmd->subticksMoveSteps.currentSize > 0) {
        CLogger::Log("cmd: {} {:#x} {:#x} {:#x}", cmd->csgoUserCmd.baseCmd->commandNumber, cmd->buttons.held, cmd->buttons.changed,
                     cmd->buttons.scroll);
        auto& steps = baseCmd->subticksMoveSteps;
        for (int i = 0; i < 12; ++i) {
            if (i >= steps.rep->allocatedSize || i >= steps.currentSize) break;
            CSubtickMoveStep* step = steps.rep->elements[i];
            CLogger::Log("subtick: {} {:#x} {} {}", i, step->button, step->when, step->pressed);
            if (step->button & IN_JUMP) {
                steps.currentSize--;
                // if i isn't the latest element, move the next elements 
                for (int j = i; j < steps.currentSize; ++j) 
                    steps.rep->elements[j] = steps.rep->elements[j + 1];
                cmd->buttons.held &= ~IN_JUMP;
                cmd->buttons.changed &= ~IN_JUMP;
                cmd->buttons.scroll &= ~IN_JUMP;
            }
        }
    }
    #endif
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

static CHook g_DrawObject;
static void hkDrawObject(void* animatableSceneObjectDesc, void* dx11, CMeshData* meshDraw, int dataCount, void* sceneView, void* sceneLayer,
                         void* unk, void* unk2) {
    if (!CEngineClient::Get()->IsInGame())
        return g_DrawObject.CallOriginal<void>(animatableSceneObjectDesc, dx11, meshDraw, dataCount, sceneView, sceneLayer, unk, unk2);

    if (!CChams::Get().OnDrawObject(animatableSceneObjectDesc, dx11, meshDraw, dataCount, sceneView, sceneLayer, unk, unk2))
        return g_DrawObject.CallOriginal<void>(animatableSceneObjectDesc, dx11, meshDraw, dataCount, sceneView, sceneLayer, unk, unk2);

    g_DrawObject.CallOriginal<void>(animatableSceneObjectDesc, dx11, meshDraw, dataCount, sceneView, sceneLayer, unk, unk2);
}

static CHook g_InputParser;
static void hkInputParser(void* inputMsg, CCSGOInputHistoryEntryPB* historyEntry, bool a3, void* a4, void* a5, C_CSPlayerPawn* pawn) {
    g_InputParser.CallOriginal<void>(inputMsg, historyEntry, a3, a4, a5, pawn);

    if (pawn == CGlobal::Get().pawn) {
        CAimbot::Get().Test(historyEntry);
    }
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
    g_DrawObject.Hook(signatures::DrawObject.GetPtrAs<void*>(), SDK_HOOK(hkDrawObject));
    g_InputParser.Hook(signatures::InputParser.GetPtrAs<void*>(), SDK_HOOK(hkInputParser));
}
