#include <pch.hpp>
#include <types/model.hpp>
#include <signatures/signatures.hpp>
#include <virtual/virtual.hpp>

void CSkeletonInstance::GetBoneData(BoneData_t& bone, int index) {
    static auto fn = signatures::GetBone.GetPtrAs<void (*)(void*, BoneData_t&, int)>();
    if (fn) fn(this, bone, index);
}

void CSkeletonInstance::CalculateWorldSpaceBones(uint32_t flags) {
    static auto fn = signatures::CalculateWorldSpaceBones.GetPtrAs<void (*)(void*, uint32_t)>();
    if (fn) fn(this, flags);
}

uint32_t CModel::GetBoneParent(int index) {
    static auto fn = signatures::BoneParent.GetPtrAs<uint32_t (*)(void*, int)>();
    return fn ? fn(this, index) : -1;
}

uint32_t CModel::GetBoneFlags(int index) {
    static auto fn = signatures::BoneFlags.GetPtrAs<uint32_t (*)(void*, int)>();
    return fn ? fn(this, index) : 0;
}

const char* CModel::GetBoneName(int index) {
    static auto fn = signatures::BoneName.GetPtrAs<const char* (*)(void*, int)>();
    return fn ? fn(this, index) : nullptr;
}

uint32_t CModel::BoneCount() { 
  static auto fn = signatures::BoneCount.GetPtrAs<uint32_t (*)(void*)>();
  return fn ? fn(this) : 0;
}
