#include <pch.hpp>
#include <hacks/visuals/visuals.hpp>

#include <cache/cache.hpp>
#include <bindings/playerpawn.hpp>
#include <bindings/cameraservices.hpp>

#include <vars/vars.hpp>

void CVisuals::OnFrameStageNotify(int stage) {
    if (stage != 6) return;

    C_CSPlayerPawn* pawn = CMatchCache::Get().GetLocalPawn();
    if (!pawn) return;

    CPlayer_CameraServices* cameraSvc = pawn->m_pCameraServices();
    if (!cameraSvc) return;

    if (g_Vars.m_MinExposure == 0.f || g_Vars.m_MaxExposure == 0.f) return;

    auto& ppVolumes = cameraSvc->m_PostProcessingVolumes();
    for (int i = 0; i < ppVolumes.m_Size; i++) {
        C_PostProcessingVolume* volume = ppVolumes.AtPtr(i)->Get();
        if (!volume) continue;

        if (minExposureBackup == 0.f) minExposureBackup = volume->m_flMinExposure();
        if (maxExposureBackup == 0.f) maxExposureBackup = volume->m_flMaxExposure();

        volume->m_flMinExposure() = g_Vars.m_MinExposure;
        volume->m_flMaxExposure() = g_Vars.m_MaxExposure;
    }
}
