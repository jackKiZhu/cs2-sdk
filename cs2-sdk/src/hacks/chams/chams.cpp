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

CMaterial2* CChams::CreateMaterial(const char* name, const char* materialVMAT, const char* shaderType, bool blendMode, bool translucent,
                                   bool disableZ) {
  CMeshData* data = std::bit_cast<CMeshData*>(std::bit_cast<byte*>(CMemAlloc::Get()->Alloc(0x200)) + 0x50);
  CMaterial2** prototype;

  CMaterialSystem::Get()->FindOrCreateFromResource(&prototype, materialVMAT);
  if (prototype == nullptr) return nullptr;
  return *prototype;
}

bool CChams::OverrideMaterial(void* animatableSceneObjectDesc, void* dx11, CMeshData* meshDraw, int dataCount, void* sceneView,
                              void* sceneLayer, void* unk, void* unk2) {
    const CMaterial2* mat = materials[MAT_TYPE_TEST];

    return true;
}
