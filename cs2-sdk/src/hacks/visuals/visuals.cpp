#include <pch.hpp>
#include <hacks/visuals/visuals.hpp>
#include <hacks/global/global.hpp>

#include <cache/cache.hpp>
#include <bindings/playerpawn.hpp>
#include <bindings/cameraservices.hpp>

#include <interfaces/globalvars.hpp>
#include <interfaces/resourcesystem.hpp>
#include <interfaces/engineclient.hpp>

#include <types/dynamiclight.hpp>

#include <vars/vars.hpp>

void CVisuals::OnFrameStageNotify(int stage) {
    C_CSPlayerPawn* pawn = CMatchCache::Get().GetLocalPawn(false);
    if (!pawn) return;

    if (stage == 6) {
        DynamicLightTest();
    }

    #if 0
    CResourceSystem* resourceSystem = CResourceSystem::Get();
    if (!resourceSystem) return;

    ResourceArray_t array_;
    resourceSystem->EnumerateResources(0x74616D76, &array_, 0x0);

    for (int i = 0; i < array_.count; i++) {
        CMaterial2* material = array_.resources[i];
        if (!material) continue;

        
    }
    #endif

    //CPlayer_CameraServices* cameraSvc = pawn->m_pCameraServices();
    //if (!cameraSvc) return;

    //const bool enabled = g_Vars.m_NightMode > 0.f;
    //const auto& ppVolumes = cameraSvc->m_PostProcessingVolumes();
    //for (int i = 0; i < ppVolumes.m_Size; i++) {
    //    C_PostProcessingVolume* volume = ppVolumes.AtPtr(i)->Get();
    //    if (!volume) continue;

    //    if (minExposureBackup == 0.f) minExposureBackup = volume->m_flMinExposure();
    //    if (maxExposureBackup == 0.f) maxExposureBackup = volume->m_flMaxExposure();
    //    
    //    volume->m_flMinExposure() = enabled ? g_Vars.m_NightMode : minExposureBackup;
    //    volume->m_flMaxExposure() = enabled ? g_Vars.m_NightMode : maxExposureBackup;
    //}

    
}

void CVisuals::DynamicLightTest() {
    void* dynamicLightManager = (void*)signatures::DynamicLightManager.GetPtr().Dereference().Get();
    static CDynamicLight* light = nullptr;

    if (!CEngineClient::Get()->IsInGame() || !dynamicLightManager || !CGlobal::Get().pawn || !CGlobal::Get().pawn->m_pGameSceneNode()) {
        light = nullptr;
        return;
    }

    static auto Create = signatures::CreateDynamicLight.GetPtrAs<CDynamicLight* (*)(void*, int, int)>();

    if (!light) {
        light = Create(dynamicLightManager, 1, 0);
    } else {
        light->origin = CGlobal::Get().pawn->m_pGameSceneNode()->m_vecOrigin();
        light->origin.z += 13.f;
        light->exponent = 40.f;
        light->radius = 350.f;
        light->color = Color_t(0, 0, 255);
        light->die = CGlobalVars::Get()->currentTime + 1000.f;
    }

}
