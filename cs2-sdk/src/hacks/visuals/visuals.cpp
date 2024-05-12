#include <pch.hpp>
#include <hacks/visuals/visuals.hpp>

#include <cache/cache.hpp>
#include <bindings/playerpawn.hpp>
#include <bindings/cameraservices.hpp>

#include <vars/vars.hpp>

void CVisuals::OnFrameStageNotify(int stage) {
    C_CSPlayerPawn* pawn = CMatchCache::Get().GetLocalPawn();
    if (!pawn) return;

    CPlayer_CameraServices* cameraSvc = pawn->m_pCameraServices();
    if (!cameraSvc) return;

    const bool enabled = g_Vars.m_NightMode > 0.f;
    const auto& ppVolumes = cameraSvc->m_PostProcessingVolumes();
    for (int i = 0; i < ppVolumes.m_Size; i++) {
        C_PostProcessingVolume* volume = ppVolumes.AtPtr(i)->Get();
        if (!volume) continue;

        if (minExposureBackup == 0.f) minExposureBackup = volume->m_flMinExposure();
        if (maxExposureBackup == 0.f) maxExposureBackup = volume->m_flMaxExposure();
        
        volume->m_flMinExposure() = enabled ? g_Vars.m_NightMode : minExposureBackup;
        volume->m_flMaxExposure() = enabled ? g_Vars.m_NightMode : maxExposureBackup;
    }
}
