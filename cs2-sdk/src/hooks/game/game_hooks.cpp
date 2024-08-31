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
#include <interfaces/memalloc.hpp>

#include <hacks/aimbot/aimbot.hpp>
#include <hacks/visuals/visuals.hpp>
#include <hacks/skinchanger/skinchanger.hpp>

#include <types/utlbuffer.hpp>

#include <interfaces/cvar.hpp>

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

    if (auto local = CMatchCache::Get().GetLocalPlayer(); local && local->IsValid(false)) {
        // if (auto pawn = local->Get()->m_hPawn().Get(); pawn && !pawn->m_bIsScoped()) {
        //     view->fov += g_Vars.m_Fov;
        //     view->viewmodelFov += g_Vars.m_ViewmodelFov;
        // }
    }

    CMath::Get().UpdateViewMatrix(pWorldToProjection);

    // SDKTODO: May sometimes stutter (?)
    //  Find a better function that won't microstutter,
    //  should also consider "host_timescale" factor.
    CESP::Get().Update();
}

static CHook g_CreateMove;
static void hkCreateMove(CCSGOInput* rcx, int subtick, char active) {
    CLagComp::Get().Reset();

    if (!CEngineClient::Get()->IsInGame()) return g_CreateMove.CallOriginal<void>(rcx, subtick, active);
    if (rcx->moves.m_Size == 0) return g_CreateMove.CallOriginal<void>(rcx, subtick, active);
    CUserCmd* cmd = rcx->GetUserCmd();
    if (!CGlobal::Get().Update(cmd)) return g_CreateMove.CallOriginal<void>(rcx, subtick, active);

    const bool grounded = CGlobal::Get().pawn->m_hGroundEntity().Get() != nullptr;

    CLagComp::Get().Update();

    auto targetData = CLagComp::Get().Find();
    CAimbot::Get().Run(targetData);

#if WORKING
    CMoveData* move = rcx->GetMoveData(0);
    if (move) {
        move->Release(IN_ATTACK, 0);

        if (CCSGOInput::IsButtonPressed(&move->buttonsHeld, IN_DUCK) && (rcx->sequenceNumber % 3) == 0) {
            move->buttonsPressed |= IN_JUMP;
            move->buttonsReleased |= IN_JUMP;
            move->buttonsHeld &= ~IN_JUMP;
            CLogger::Log("{}: Jump", rcx->sequenceNumber);
        }

        // move->buttonsHeld &= ~IN_JUMP;
        // move->buttonsPressed &= ~IN_JUMP;
        // move->buttonsReleased &= ~IN_JUMP;
    }
#else

    // rcx->Scroll(IN_JUMP);
    // rcx->Release(IN_ATTACK);



#endif

    // CLogger::Log("Pre checksum: {}", (uintptr_t)cmd->csgoUserCmd.baseCmd->moveCRC);

    g_CreateMove.CallOriginal<bool>(rcx, subtick, active);

    // CLagComp::Get().Test();

    // const CInButtonState oldButtons = cmd->buttons;
// modify
#if 0
    cmd->buttons.scroll |= IN_JUMP;
    
    if (!cmd->csgoUserCmd.baseCmd->ComputeCRC()) {
        cmd->buttons = oldButtons;
    } else {
        cmd->GetBaseCmdButtons();
        // maybe set ccsgoinput buttons
    }
#endif

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

static void hkDrawArray(ISceneObjectDesc* const desc, IRenderContext* ctx, CMeshDrawPrimitive_t* renderList, int numRenderablesToDraw,
                        const ISceneView* view, ISceneLayer* layer, SceneSystemPerFrameStats_t* const perFrameStats,
                        const CMaterial2* material) {
    if (!CEngineClient::Get()->IsInGame())
        return CGameHooks::Get().g_DrawArray.CallOriginal<void>(desc, ctx, renderList, numRenderablesToDraw, view, layer, perFrameStats,
                                                                material);

    if (!CChams::Get().OnDrawObject(desc, ctx, renderList, numRenderablesToDraw, view, layer, perFrameStats, material))
        return CGameHooks::Get().g_DrawArray.CallOriginal<void>(desc, ctx, renderList, numRenderablesToDraw, view, layer, perFrameStats,
                                                                material);
}

static CHook g_InputParser;
static void hkInputParser(CInputFrame* inputMsg, CCSGOInputHistoryEntryPB* historyEntry, bool attackOnly, void* a4, void* a5,
                          C_CSPlayerPawn* pawn) {
    return g_InputParser.CallOriginal<void>(inputMsg, historyEntry, attackOnly, a4, a5, pawn);

    if (pawn == CGlobal::Get().pawn) {
        CLagComp::Get().PreModifyInput(inputMsg, historyEntry);
    }

    g_InputParser.CallOriginal<void>(inputMsg, historyEntry, attackOnly, a4, a5, pawn);

    // restore player tick here

    if (pawn == CGlobal::Get().pawn) {
        CLagComp::Get().PostModifyInput(inputMsg, historyEntry);
    }
}

// ctor
static CHook g_CAnimationGraphInstance;
static void* hkCAnimationGraphInstance(void* rcx, void* rdx, void* data) {
    if (const char* model = *(const char**)((uintptr_t)data + 0x8); model && strstr(model, "weapons/models/knife")) {
        void* m_animGraphNetworkedVars = *(void**)((uintptr_t)rdx + 0x30);
        m_animGraphNetworkedVars = nullptr;
        // CLogger::Log("CAnimationGraphInstance: {}", model);
    }

    return g_CAnimationGraphInstance.CallOriginal<void*>(rcx, rdx, data);
}

static CHook g_UpdateAggregateSceneObject;
static void* hkUpdateAggregateSceneObject(void* rcx, void* rdx) {
    void* result = g_UpdateAggregateSceneObject.CallOriginal<void*>(rcx, rdx);

    // quite literally is only going to be accessed here
    class CAggregateSceneObjectData {
       public:
        PAD(0x38);
        Color_t color;
        PAD(0x8);
    };

    class CAggregateSceneObject {
       public:
        PAD(0x120);
        int count;
        PAD(0x4);
        CAggregateSceneObjectData* data;
    };

    CAggregateSceneObject* obj = (CAggregateSceneObject*)rcx;
    for (int i = 0; i < obj->count; ++i) {
        CAggregateSceneObjectData& data = obj->data[i];
        data.color = g_Vars.m_WorldColor;
    }

    return result;
}

static CHook g_PlayerPingRender;
static void hkPlayerPingRender(void* a1, char a2) {
    g_PlayerPingRender.CallOriginal<void>(a1, a2);

    CLogger::Log("PlayerPingRender: {:#x} {} called from {:#x}", uintptr_t(a1), a2, uintptr_t(_ReturnAddress()));
}

static CHook g_GlowServices;
static bool hkGlowServices(void* e, void* local, float* clr, float* spike, float* silent, float* endTime1, float* endTime2, bool* glow) {
    bool result = g_GlowServices.CallOriginal<bool>(e, local, clr, spike, silent, endTime1, endTime2, glow);
    if (!CEngineClient::Get()->IsInGame() || !CGlobal::Get().pawn) return result;

    if (g_Vars.m_EnableIfSpectating && CGlobal::Get().pawn->IsObserverPawn()) {
        *glow = true;
        return true;
    }

    return result;
}

static CHook g_SampleInput;
static void hkSampleInput(CCSGOInput* input, int subtick, float angleSpeed, char a4) {
    g_SampleInput.CallOriginal<bool>(input, subtick, angleSpeed, a4);

    CMoveData* move = input->GetMoveData(0);
    // if (move && false) {
    //     move->buttonsHeld &= ~IN_ATTACK;
    //     move->buttonsPressed &= ~IN_ATTACK;
    //     move->buttonsReleased &= ~IN_ATTACK;

    //    if (CCSGOInput::IsButtonPressed(&move->buttonsHeld, IN_JUMP) /*&& !(move->prevButtonsHeld & IN_JUMP)*/ &&
    //        (input->sequenceNumber % 3) == 0) {
    //        move->buttonsPressed |= IN_JUMP;

    //        move->buttonsReleased |= IN_JUMP;
    //        move->buttonsHeld &= ~IN_JUMP;
    //        CLogger::Log("{}: Jump", input->sequenceNumber);
    //    }

    //    move->buttonsHeld &= ~IN_JUMP;
    //    move->buttonsPressed &= ~IN_JUMP;
    //    move->buttonsReleased &= ~IN_JUMP;
    //}
}

static CHook g_GetConvarValue;
static void* hkGetConvarValue(void* rcx, int a2) {
    static bool t = true;
    ConVar* cvar = *(ConVar**)(uintptr_t(rcx) + 0x8);
    uint32_t hash = FNV1A::Hash(cvar->m_Name);
    // if (hash == FNV1A::HashConst("cl_showusercmd")) {
    //     return &t;
    // }

    return g_GetConvarValue.CallOriginal<void*>(rcx, a2);
}

void CGameHooks::Initialize() {
    SDK_LOG_PROLOGUE();

    CMatchCache::Get().Initialize();

    g_MouseInputEnabled.VHook(CCSGOInput::Get(), platform::Constant(16, 16), SDK_HOOK(hkMouseInputEnabled));
    // g_SampleInput.VHook(CCSGOInput::Get(), 4, SDK_HOOK(hkSampleInput));
    g_CreateMove.VHook(CCSGOInput::Get(), 5, SDK_HOOK(hkCreateMove));
    g_OnAddEntity.VHook(CGameEntitySystem::Get(), platform::Constant(14, 15), SDK_HOOK(hkOnAddEntity));
    g_OnRemoveEntity.VHook(CGameEntitySystem::Get(), platform::Constant(15, 16), SDK_HOOK(hkOnRemoveEntity));
    g_FrameStageNotify.VHook(CSource2Client::Get(), platform::Constant(36, 37), SDK_HOOK(hkFrameStageNotify));
    g_EquipItemInLoadout.VHook(CCSInventoryManager::Get(), platform::Constant(56, 56), SDK_HOOK(hkEquipItemInLoadout));
    g_GetMatricesForView.Hook(signatures::GetMatricesForView.GetPtrAs<void*>(), SDK_HOOK(hkGetMatricesForView));
    g_FireEventClientSide.Hook(signatures::FireEventClientSide.GetPtrAs<void*>(), SDK_HOOK(hkFireEventClientSide));
    g_SetModel.Hook(signatures::SetModel.GetPtrAs<void*>(), SDK_HOOK(hkSetModel));
    g_IsLoadoutAllowed.Hook(signatures::IsLoadoutAllowed.GetPtrAs<void*>(), SDK_HOOK(hkIsLoadoutAllowed));
    g_DrawArray.Hook(signatures::DrawObject.GetPtrAs<void*>(), SDK_HOOK(hkDrawArray));
    g_InputParser.Hook(signatures::InputParser.GetPtrAs<void*>(), SDK_HOOK(hkInputParser));
    g_CAnimationGraphInstance.Hook(signatures::CAnimationGraphInstance.GetPtrAs<void*>(), SDK_HOOK(hkCAnimationGraphInstance));
    g_UpdateAggregateSceneObject.Hook(signatures::UpdateAggregateSceneObject.GetPtrAs<void*>(), SDK_HOOK(hkUpdateAggregateSceneObject));
    g_GlowServices.Hook(signatures::GlowServices.GetPtrAs<void*>(), SDK_HOOK(hkGlowServices));
    g_GetConvarValue.Hook(signatures::GetConvarValue.GetPtrAs<void*>(), SDK_HOOK(hkGetConvarValue));
    //  g_PlayerPingRender.Hook(signatures::PlayerPingRender.GetPtrAs<void*>(), SDK_HOOK(hkPlayerPingRender));
}

class CTakeDamageInfo {
   public:
    void* vftable;
    Vector m_vecDamageForce;
    Vector m_vecDamagePosition;
    Vector m_vecReportedPosition;
    Vector m_vecDamageDirection;
    CHandle<C_BaseEntity> m_hInflictor;
    CHandle<C_BaseEntity> m_hAttacker;
    CHandle<C_BaseEntity> m_hWeapon;
    float m_flDamage;
    float m_flMaxDamage;
    float m_flBaseDamage;
    int m_bitsDamageType;
    int m_iDamageCustom;
    int m_iAmmoType;
    float m_flRadius;
    void* m_hBodyPart;
    bool m_bCanHeadshot;
    float m_flOriginalDamage;
    int m_nDamageTaken;
    int16_t m_iRecord;
    float m_flStabilityDamage;
    int m_bitsDotaDamageType;
    int m_nDotaDamageCategory;
    bool m_bAllowFriendlyFire;
    bool m_bCanBeBlocked;
    void* m_hScriptInstance;
};
