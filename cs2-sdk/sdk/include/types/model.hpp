#pragma once

#include <schemamgr/schema_manager.hpp>

#include <math/types/vector.hpp>

struct BoneData_t
{
    Vector position;
    float scale;
    float rotation[4];
};

class CModelState {
   public:
    PAD(0x80);
    BoneData_t* bones;
};

class CSkeletonInstance
{
   public:
    SCHEMA(CModelState, m_modelState, "CSkeletonInstance", "m_modelState");

    void GetBone(BoneData_t& bone, int index);
};
