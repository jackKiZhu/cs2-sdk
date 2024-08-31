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
    static constexpr char bloom[] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
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

    static constexpr char glow2[] = R"#(
            <!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_unlitgeneric.vfx"
                g_tColor = resource:"materials/decals/snow01_color_tga_b05c3296.vtex"
                g_tNormal = resource:"materials/ground/snow01_normal_psd_b04107b2.vtex"
                g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                g_tMetalness = resource:"characters/models/shared/materials/animset_t_vmat_g_tmetalness_5be93c08.vtex"
                g_tAmbientOcclusion = resource:"materials/models/particle/fleks_snow_01_vmat_g_tambientocclusion_7446266e.vtex"
                g_vTexCoordScrollSpeed = [0.100000, 0.000000, 0.000000, 0.000000]
                TextureRoughness = [0.01961, 0.101961, 0.101961, 0.000000]
                g_flPearlescentScale = 1
                g_flMetalness = 10.0
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
                F_IGNOREZ = 0
                F_DISABLE_Z_WRITE = 0
                F_DISABLE_Z_BUFFERING = 0
                F_RENDER_BACKFACES = 1
                g_vColorTint = [1.0, 1.0, 1.0, 0.0]
            }     
)#";

    static constexpr char glow[] = R"#(
<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_effects.vfx"
                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
                g_tMask1 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask2 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask3 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_flOpacityScale = 0.45
                g_flFresnelExponent = 0.75
                g_flFresnelFalloff = 1
                g_flFresnelMax = 0.0
                g_flFresnelMin = 1
                F_ADDITIVE_BLEND = 1
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
                F_IGNOREZ = 0
                F_DISABLE_Z_WRITE = 0
                F_DISABLE_Z_BUFFERING = 0
                F_RENDER_BACKFACES = 1
                g_vColorTint = [1.0, 1.0, 1.0, 0.0]
            }
)#";

    CKeyValues3 bloomKV(bloom);
    CKeyValues3 glow2KV(glow2);
    CKeyValues3 glowKV(glow);
    CStrongHandle<CMaterial2> mat;

    CMaterialSystem::Get()->CreateMaterial(&mat, "bloom", &bloomKV, 0, 1);
    materials[MAT_BLOOM] = mat.Get();

    CMaterialSystem::Get()->CreateMaterial(&mat, "glow2", &glow2KV, 0, 1);
    materials[MAT_GLOW2] = mat.Get();

    CMaterialSystem::Get()->CreateMaterial(&mat, "glow", &glowKV, 0, 1);
    materials[MAT_GLOW] = mat.Get();
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

    CBaseHandle hOwner = renderList->m_pObject->m_hExternalOwner;
    if (!hOwner.IsValid()) return false;
    C_CSPlayerPawn* pawn = CGameResourceService::Get()->GetGameEntitySystem()->GetBaseEntity<C_CSPlayerPawn>(hOwner.GetEntryIndex());
    if (!pawn || !pawn->IsPlayerPawn() || pawn->m_iTeamNum() == CGlobal::Get().pawn->m_iTeamNum()) return false;

    // if (renderList) CLogger::Log("DrawArray: {:#x}", (uintptr_t)renderList->m_pObject);

    // DrawBacktrack(pawn, renderList->m_pTransform);
    [&]() {
        return;
        const CSceneAnimatableObject* obj = renderList->m_pObject;
        const TargetData_t& lagcomp = CLagComp::Get().data;
        if (!lagcomp.player || pawn != lagcomp.pawn) return;
        if (lagcomp.player->records.empty()) return;
        const auto& boneMatrix = lagcomp.records->front().boneMatrix;
        if (boneMatrix.size() != obj->m_worldSpaceRenderBones.m_Size) return;
        matrix3x4a_t* backup = std::bit_cast<matrix3x4a_t*>(_malloca(obj->m_worldSpaceRenderBones.m_Size * sizeof(matrix3x4a_t)));
        if (!backup) return;
        memcpy(backup, obj->m_worldSpaceRenderBones.m_Data, obj->m_worldSpaceRenderBones.m_Size * sizeof(matrix3x4a_t));

        // matrix4x2 -> matrix3x4
        for (size_t i = 0; i < boneMatrix.size(); ++i) {
            const BoneData_t& data = boneMatrix.at(i);
            obj->m_worldSpaceRenderBones.m_Data[i] = data.ToMatrix();
        }

        OverrideMaterial(desc, ctx, renderList, numRenderablesToDraw, view, layer, perFrameStats, material);

        memcpy(obj->m_worldSpaceRenderBones.m_Data, backup, obj->m_worldSpaceRenderBones.m_Size * sizeof(matrix3x4a_t));
        _freea(backup);
    }();

    return OverrideMaterial(desc, ctx, renderList, numRenderablesToDraw, view, layer, perFrameStats, material);
}

bool CChams::OverrideMaterial(ISceneObjectDesc* const desc, IRenderContext* ctx, CMeshDrawPrimitive_t* renderList, int numRenderablesToDraw,
                              const ISceneView* view, ISceneLayer* layer, SceneSystemPerFrameStats_t* const perFrameStats,
                              const CMaterial2* material) {
    renderList->m_pMaterial = materials[g_Vars.m_ChamsMat];
    renderList->m_rgba = g_Vars.m_ChamsColor;
    CGameHooks::Get().g_DrawArray.CallOriginal<void>(desc, ctx, renderList, numRenderablesToDraw, view, layer, perFrameStats, material);
    return true;
}
