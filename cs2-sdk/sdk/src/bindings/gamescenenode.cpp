#include <pch.hpp>

#include <schemamgr/schema_manager.hpp>

#include <math/types/transform.hpp>

#include <bindings/gamescenenode.hpp>
#include <signatures/signatures.hpp>

void CGameSceneNode::SetMeshGroupMask(uint64_t meshGroupMask) {
    static auto SetMeshGroupMask = signatures::SetMeshGroupMask.GetPtrAs<void(*)(CGameSceneNode*, uint64_t)>();
    if (SetMeshGroupMask)
		SetMeshGroupMask(this, meshGroupMask);
}
