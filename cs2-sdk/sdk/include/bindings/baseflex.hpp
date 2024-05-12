#pragma once

#include <bindings/baseentity.hpp>
#include <types/handle.hpp>

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

// i'm not making files for the fucking sky
class C_EnvSky : public C_BaseModelEntity {
   public:
    // SCHEMA(CStrongHandle<, m_pGameSceneNode, "C_BaseEntity", "m_pGameSceneNode");
};
