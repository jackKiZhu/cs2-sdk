#pragma once

#include <bindings/baseentity.hpp>
#include <types/handle.hpp>

class C_TonemapController2
{
   public:
    SCHEMA(float, m_flAutoExposureMin, "C_TonemapController2", "m_flAutoExposureMin");
    SCHEMA(float, m_flAutoExposureMax, "C_TonemapController2", "m_flAutoExposureMax");
    SCHEMA(float, m_flTonemapPercentTarget, "C_TonemapController2", "m_flTonemapPercentTarget");
    SCHEMA(float, m_flTonemapPercentBrightPixels, "C_TonemapController2", "m_flTonemapPercentBrightPixels");
    SCHEMA(float, m_flTonemapMinAvgLum, "C_TonemapController2", "m_flTonemapMinAvgLum");
    SCHEMA(float, m_flExposureAdaptationSpeedUp, "C_TonemapController2", "m_flExposureAdaptationSpeedUp");
    SCHEMA(float, m_flExposureAdaptationSpeedDown, "C_TonemapController2", "m_flExposureAdaptationSpeedDown");
    SCHEMA(float, m_flTonemapEVSmoothingRange, "C_TonemapController2", "m_flTonemapEVSmoothingRange");
};

class C_PostProcessingVolume
{
   public:
    SCHEMA(float, m_flMinExposure, "C_PostProcessingVolume", "m_flMinExposure");
    SCHEMA(float, m_flMaxExposure, "C_PostProcessingVolume", "m_flMaxExposure");
};

class C_PostProcessingVolume;

class CPlayer_CameraServices {
   public:
    SCHEMA(CHandle<C_BaseEntity>, m_hViewEntity, "CPlayer_CameraServices", "m_hViewEntity");
    SCHEMA(CUtlVector<CHandle<C_PostProcessingVolume>>, m_PostProcessingVolumes, "CPlayer_CameraServices", "m_PostProcessingVolumes");
    SCHEMA(CHandle<C_PostProcessingVolume>, m_hActivePostProcessingVolume, "CPlayer_CameraServices", "m_hActivePostProcessingVolume");
};
