#include <pch.hpp>

#include <schemamgr/schema_manager.hpp>

#include <math/types/transform.hpp>
#include <types/model.hpp>

#include <bindings/gamescenenode.hpp>
#include <signatures/signatures.hpp>
#include <virtual/virtual.hpp>

void CGameSceneNode::SetMeshGroupMask(uint64_t meshGroupMask) {
    static auto SetMeshGroupMask = signatures::SetMeshGroupMask.GetPtrAs<void(*)(CGameSceneNode*, uint64_t)>();
    if (SetMeshGroupMask)
		  SetMeshGroupMask(this, meshGroupMask);
}

CSkeletonInstance* CGameSceneNode::GetSkeleton() { return vt::CallMethod<CSkeletonInstance*>(this, 8); }
