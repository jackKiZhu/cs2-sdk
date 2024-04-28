#pragma once

#include <math/types/transform.hpp>

class CSkeletonInstance;

class CGameSceneNode {
   public:
    SCHEMA(CTransform, m_nodeToWorld, "CGameSceneNode", "m_nodeToWorld");

    void SetMeshGroupMask(uint64_t meshGroupMask);
    CSkeletonInstance* GetSkeleton();
};
