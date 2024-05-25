#include <pch.hpp>

#include <hacks/chams/chams.hpp>
#include <hacks/global/global.hpp>
#include <hacks/aimbot/lagcomp.hpp>

#include <interfaces/memalloc.hpp>
#include <interfaces/materialsystem.hpp>
#include <interfaces/gameresourceservice.hpp>
#include <interfaces/gameentitysystem.hpp>

#include <bindings/baseentity.hpp>
#include <bindings/playercontroller.hpp>
#include <bindings/playerpawn.hpp>

#include <types/model.hpp>

#include <vars/vars.hpp>

#include <cache/cache.hpp>
#include <cache/entities/player.hpp>

#include <hooks/game/game_hooks.hpp>

#include <logger/logger.hpp>

void CChams::Initialize() {
    static constexpr char glowVisible[] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
			format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
			{
                shader = "solidcolor.vfx"
                F_SELF_ILLUM = 1
                F_PAINT_VERTEX_COLORS = 1
                F_TRANSLUCENT = 1 
                F_IGNOREZ = 0
                F_DISABLE_Z_WRITE = 0
                F_DISABLE_Z_BUFFERING = 0
               
                g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
				g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                g_tSelfIllumMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                TextureAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
                g_tAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
                
                g_vColorTint = [ 20.000000, 20.000000, 20.000000, 20.000000 ]
                g_flSelfIllumScale = [ 5.000000, 5.000000, 5.000000, 5.000000 ]
                g_flSelfIllumBrightness = [ 5.000000, 5.000000, 5.000000, 5.000000 ]
                g_vSelfIllumTint = [ 10.000000, 10.000000, 10.000000, 10.000000 ]
			} )#";

    void* buffer = CMemAlloc::Get().Alloc(0x100 + sizeof(CKeyValues));
    memset(buffer, 0, 0x100 + sizeof(CKeyValues));
    CKeyValues* kv = std::bit_cast<CKeyValues*>(uintptr_t(buffer) + 0x100);
    KVID_t id;
    id.name = "generic";
    id.pad[0] = 0x469806E97412167C;
    id.pad[1] = 0xE73790B53EE6F2AF;

    if (!kv->Load(glowVisible, &id, nullptr)) return; // crasj

    CStrongHandle<CMaterial2> mat;
    CMaterialSystem::Get()->CreateMaterial(&mat, "glow_vis", kv, 0, 1);
    materials[MAT_FLAT] = mat.Get();
    // CreateMaterial("primary_white", "materials/dev/primary_white.vmat", "csgo_unlitgeneric.vfx", true, true, false);
    // materials[MAT_FLAT_Z] = CreateMaterial("primary_white_z", "materials/dev/primary_white.vmat", "csgo_unlitgeneric.vfx", true, true,
    // true); materials[MAT_REGULAR] = mat.Get(); // CreateMaterial("regular_white", "materials/dev/primary_white.vmat",
    // "csgo_litgeneric.vfx", true, true, false); materials[MAT_REGULAR_Z] = CreateMaterial("regular_white_z",
    // "materials/dev/primary_white.vmat", "csgo_litgeneric.vfx", true, true, true);
}

void CChams::Shutdown() {}

bool CChams::IsEnabled() { return g_Vars.m_Chams; }

bool CChams::OnDrawObject(void* animatableSceneObjectDesc, void* dx11, CSceneData* meshDraw, int dataCount, void* sceneView,
                          void* sceneLayer, void* unk, void* unk2) {
    if (!IsEnabled() || !CGlobal::Get().pawn || !meshDraw || !meshDraw->sceneAnimatableObject) return false;
    CBaseHandle hOwner = meshDraw->sceneAnimatableObject->ownerHandle;
    if (!hOwner.IsValid()) return false;
    C_CSPlayerPawn* entity = CGameResourceService::Get()->GetGameEntitySystem()->GetBaseEntity<C_CSPlayerPawn>(hOwner.GetEntryIndex());
    if (!entity || !entity->IsPlayerPawn() || entity->m_iTeamNum() == CGlobal::Get().pawn->m_iTeamNum()) return false;

    auto skeleton = entity->m_pGameSceneNode()->GetSkeleton();

    auto ptr = (uintptr_t)meshDraw->sceneAnimatableObject;
    // scan some bytes near ptr to find some specific data
    //for (int i = 0; i < 0x500; i++) {
    //    if (*(uintptr_t*)(ptr + i) == (uintptr_t)skeleton) {
    //        CLogger::Log("Found skeleton at offset {}", i);
    //        break;
    //    }

    //    if (*(uintptr_t*)(ptr + i) == (uintptr_t)entity->m_pGameSceneNode()) {
    //        CLogger::Log("Found skeleton at offset {}", i);
    //        break;
    //    }
    //}

    // CSkeletonInstance* skeleton = meshDraw->sceneAnimatableObject->skeleton;
    // if (!skeleton || !skeleton->m_modelState().bones) return false;
    // CModel* model = skeleton->m_modelState().m_hModel().Get();
    // if (!model) return false;
    // uint32_t boneCount = model->BoneCount();
    // if (boneCount == 0) return false;

    // if (entity == CLagComp::Get().data.pawn && CLagComp::Get().data.bestRecord) {
    //     // keep track of the original pointer, replace with best record pointer matrix then restore
    //     BoneData_t* backup = skeleton->m_modelState().bones;
    //     skeleton->m_modelState().bones = CLagComp::Get().data.bestRecord->boneMatrix.data();
    //     OverrideMaterial(animatableSceneObjectDesc, dx11, meshDraw, dataCount, sceneView, sceneLayer, unk, unk2);
    //     CGameHooks::Get().g_DrawObject.CallOriginal<void>(animatableSceneObjectDesc, dx11, meshDraw, dataCount, sceneView, sceneLayer,
    //     unk, unk2); skeleton->m_modelState().bones = backup;
    // }

    return OverrideMaterial(animatableSceneObjectDesc, dx11, meshDraw, dataCount, sceneView, sceneLayer, unk, unk2);
}

CStrongHandle<CMaterial2> CChams::CreateMaterial(const char* name) {
    static constexpr char vmtBuffer[] =
        R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
	{
		Shader = "csgo_unlitgeneric.vfx"
		F_DISABLE_Z_BUFFERING = 0
		g_flBumpStrength = 1
		g_vColorTint = [1.000000, 1.000000, 1.000000, 0.000000]
		g_vGlossinessRange = [0.000000, 1.000000, 0.000000, 0.000000]
		g_vNormalTexCoordScale = [1.000000, 1.000000, 0.000000, 0.000000]
		g_vTexCoordOffset = [0.000000, 0.000000, 0.000000, 0.000000]
		g_vTexCoordScale = [1.000000, 1.000000, 0.000000, 0.000000]
		g_tColor = resource:"materials/dev/primary_white_color_tga_f7b257f6.vtex"
		g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
		g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
	})";

    CKeyValues* kv = std::bit_cast<CKeyValues*>(CMemAlloc::Get().Alloc(0x100 + sizeof(CKeyValues)));
    KVID_t id;
    id.name = "generic";
    id.pad[0] = 0x469806E97412167C;
    id.pad[1] = 0xE73790B53EE6F2AF;

    if (!kv->Load(vmtBuffer, &id, nullptr)) return nullptr;

    CStrongHandle<CMaterial2> mat;
    CMaterialSystem::Get()->CreateMaterial(&mat, name, kv, 0, 1);
    return mat;
}

CMaterial2* CChams::CreateMaterial(const char* name, const char* materialVMAT, const char* shaderType, bool blendMode, bool translucent,
                                   bool disableZ) {
    CSceneData* data = (CSceneData*)(std::bit_cast<byte*>(CMemAlloc::Get().Alloc(0x200)) + 0x50);
    CMaterial2** prototype;

    CMaterialSystem::Get()->FindOrCreateFromResource(&prototype, materialVMAT);
    if (prototype == nullptr) {
        CMemAlloc::Get().Free(data);
        return nullptr;
    }

    CMaterialSystem::Get()->SetCreateDataByMaterial(&prototype, data);
    data->SetShaderType(shaderType);
    data->SetMaterialFunction("F_DISABLE_Z_BUFFERING", disableZ ? 1 : 0);
    data->SetMaterialFunction("F_BLEND_MODE", blendMode ? 1 : 0);
    data->SetMaterialFunction("F_TRANSLUCENT", translucent ? 1 : 0);

    CMaterial2** mat;
    CMaterialSystem::Get()->CreateMaterial(&mat, name, data, 0, 1);
    CMemAlloc::Get().Free(data);
    return *mat;
}

bool CChams::OverrideMaterial(void* animatableSceneObjectDesc, void* dx11, CSceneData* meshDraw, int dataCount, void* sceneView,
                              void* sceneLayer, void* unk, void* unk2) {
    CMaterial2* mat = materials[MAT_FLAT];
    meshDraw->material = mat;
    meshDraw->color = g_Vars.m_ChamsColor;
    return true;
}
