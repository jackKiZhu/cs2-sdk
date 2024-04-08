#pragma once

#include <math/types/transform.hpp>

class CGameSceneNode {
   public:
    SCHEMA(CTransform, m_nodeToWorld, "CGameSceneNode", "m_nodeToWorld");

    void SetMeshGroupMask(uint64_t meshGroupMask);
};
