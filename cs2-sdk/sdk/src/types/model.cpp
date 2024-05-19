#include <pch.hpp>
#include <types/model.hpp>
#include <signatures/signatures.hpp>
#include <virtual/virtual.hpp>

void CSkeletonInstance::GetBone(BoneData_t& bone, int index) {
    static auto fn = signatures::GetBone.GetPtrAs<void (*)(BoneData_t&, int)>();
    if (fn) fn(bone, index);
}
