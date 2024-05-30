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

#include <math/math.hpp>

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

    if (!kv->Load(glowVisible, &id, nullptr)) return;  // crasj

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

static void DrawBacktrack(C_CSPlayerPawn* pawn, matrix3x4_t* matrix) {
    const TargetData_t& lagcomp = CLagComp::Get().data;
    if (!lagcomp.player || pawn != lagcomp.pawn) return;
    if (lagcomp.player->records.empty()) return;
    const auto& boneMatrix = lagcomp.records->front().boneMatrix;
    if (boneMatrix.empty() || boneMatrix.size() <= BONE_ROOT_MOTION) return;
    const BoneData_t& data = boneMatrix.at(BONE_ROOT_MOTION);
    const matrix3x4_t backup = *matrix;

    matrix->SetAxis(3, data.position);

    *matrix = backup;
}

bool CChams::OnDrawObject(ISceneObjectDesc* const desc, IRenderContext* ctx, CMeshDrawPrimitive_t* renderList, int numRenderablesToDraw,
                          const ISceneView* view, ISceneLayer* layer, SceneSystemPerFrameStats_t* const perFrameStats,
                          const CMaterial2* material) {
    if (!IsEnabled() || !CGlobal::Get().pawn || !renderList || !renderList->m_pObject) return false;

    CBaseHandle hOwner = renderList->m_pObject->ownerHandle;
    if (!hOwner.IsValid()) return false;
    C_CSPlayerPawn* pawn = CGameResourceService::Get()->GetGameEntitySystem()->GetBaseEntity<C_CSPlayerPawn>(hOwner.GetEntryIndex());
    if (!pawn || !pawn->IsPlayerPawn() || pawn->m_iTeamNum() == CGlobal::Get().pawn->m_iTeamNum()) return false;

    // DrawBacktrack(pawn, renderList->m_pTransform);
    [&]() {
        const TargetData_t& lagcomp = CLagComp::Get().data;
        if (!lagcomp.player || pawn != lagcomp.pawn) return;
        if (lagcomp.player->records.empty()) return;
        const auto& boneMatrix = lagcomp.records->front().boneMatrix;
        if (boneMatrix.empty() || boneMatrix.size() <= BONE_ROOT_MOTION) return;
        const BoneData_t& data = boneMatrix.at(BONE_ROOT_MOTION);
        const matrix3x4_t backup = *renderList->m_pTransform;

        renderList->m_pTransform->SetAxis(3, data.position);
        OverrideMaterial(desc, ctx, renderList, numRenderablesToDraw, view, layer, perFrameStats, material);

        *renderList->m_pTransform = backup;
    }();

    return OverrideMaterial(desc, ctx, renderList, numRenderablesToDraw, view, layer, perFrameStats, material);
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
    CMeshDrawPrimitive_t* data = (CMeshDrawPrimitive_t*)(std::bit_cast<byte*>(CMemAlloc::Get().Alloc(0x200)) + 0x50);
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

bool CChams::OverrideMaterial(ISceneObjectDesc* const desc, IRenderContext* ctx, CMeshDrawPrimitive_t* renderList, int numRenderablesToDraw,
                              const ISceneView* view, ISceneLayer* layer, SceneSystemPerFrameStats_t* const perFrameStats,
                              const CMaterial2* material) {
    CMaterial2* mat = materials[MAT_FLAT];
    renderList->m_pMaterial = mat;
    renderList->m_rgba = g_Vars.m_ChamsColor;
    CGameHooks::Get().g_DrawArray.CallOriginal<void>(desc, ctx, renderList, numRenderablesToDraw, view, layer, perFrameStats, material);
    return true;
}
