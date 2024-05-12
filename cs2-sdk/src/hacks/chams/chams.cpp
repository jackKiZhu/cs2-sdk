#include <pch.hpp>

#include <hacks/chams/chams.hpp>

#include <interfaces/memalloc.hpp>
#include <interfaces/materialsystem.hpp>
#include <interfaces/gameresourceservice.hpp>
#include <interfaces/gameentitysystem.hpp>

#include <bindings/baseentity.hpp>
#include <bindings/playercontroller.hpp>
#include <bindings/playerpawn.hpp>

void CChams::Initialize() {
    materials[MAT_TYPE_TEST] = CreateMaterial("primary_white", "materials/dev/primary_white.vmat", "csgo_unlitgeneric.vfx", true, true, false);
}

void CChams::Shutdown() {}

bool CChams::IsEnabled() { return true; }

bool CChams::OnDrawObject(void* animatableSceneObjectDesc, void* dx11, CMeshData* meshDraw, int dataCount, void* sceneView,
                          void* sceneLayer, void* unk, void* unk2) {
    if (!IsEnabled() || !meshDraw || !meshDraw->sceneAnimatableObject) return false;
    CBaseHandle hOwner = meshDraw->sceneAnimatableObject->owner;
    if (!hOwner.IsValid()) return false;
    C_CSPlayerPawn* entity = CGameResourceService::Get()->GetGameEntitySystem()->GetBaseEntity<C_CSPlayerPawn>(hOwner.GetEntryIndex());
    if (!entity || !entity->IsPlayerPawn()) return false;
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
  void* a = CMemAlloc::Get().Alloc(0x200);


  CMeshData* data = (CMeshData*)(std::bit_cast<byte*>(CMemAlloc::Get().Alloc(0x200)) + 0x50);
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

  CMemAlloc::Get().Free(data);
  return *prototype;
}

bool CChams::OverrideMaterial(void* animatableSceneObjectDesc, void* dx11, CMeshData* meshDraw, int dataCount, void* sceneView,
                              void* sceneLayer, void* unk, void* unk2) {
    CMaterial2* mat = materials[MAT_TYPE_TEST];
    meshDraw->material = mat;
    meshDraw->color = Color_t(0, 122, 220);

    return true;
}
