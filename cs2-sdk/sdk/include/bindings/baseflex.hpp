#pragma once
#include <bindings/baseentity.hpp>

class C_BaseModelEntity : public C_BaseEntity {
   public:
    void SetModel(const char* model);
};

class CBaseAnimGraph : public C_BaseModelEntity {
   public:
};

class C_BaseFlex : public CBaseAnimGraph {
   public:
};
