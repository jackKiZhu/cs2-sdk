#pragma once

#include <math/types/transform.hpp>

class CSkeletonInstance;

struct InterpInfo_t
{
    int srcTick;
    int dstTick;
    float fraction;
};

struct Tickfrac_t
{
    int tick;
    float fraction;
};

class CGameSceneNode {
   public:
    SCHEMA(CTransform, m_nodeToWorld, "CGameSceneNode", "m_nodeToWorld");
    SCHEMA(Vector, m_angRotation, "CGameSceneNode", "m_angRotation");
    SCHEMA(Vector, m_vecOrigin, "CGameSceneNode", "m_vecOrigin");
    SCHEMA(Vector, m_vecAbsOrigin, "CGameSceneNode", "m_vecAbsOrigin");
    SCHEMA(Vector, m_angAbsRotation, "CGameSceneNode", "m_angAbsRotation");
    SCHEMA(Vector, m_vRenderOrigin, "CGameSceneNode", "m_vRenderOrigin");

    void SetMeshGroupMask(uint64_t meshGroupMask);
    CSkeletonInstance* GetSkeleton();

    // 0xF4 == valid skeleton instance
    bool CalculateInterpInfos(InterpInfo_t* cl, InterpInfo_t* sv0, InterpInfo_t* sv1, Tickfrac_t* pl);
};
