#pragma once

#include <types/color.hpp>
#include <math/types/vector.hpp>
#include <math/types/matrix3x4.hpp>
#include <bindings/baseentity.hpp>

class ISceneObjectDesc {
   public:
    void* vftable;
};

class CRenderAttributes {
   public:
    PAD(0x320);
};

class IRenderContext {
   public:
    void* vftable;
    IRenderContext* primaryContext;
    CRenderAttributes attributes;
    const void* materialStateHintMode;
    void* materialStateHintTexture;
    uint64_t materialStateHintKey;
};

class CMeshInstance;
class CSceneObject;
class CSceneSystemStencilState;
class CSceneSystemBakedLightingInfo;
class CMaterial2;

#pragma pack(push, 1)
class CSceneObject {
   public:
    void* vftable;              // 0x0
    void* heapPtr;              // 0x8
    void* m_pMeshInstanceData;  // 0x10
    void* m_pDesc;              // 0x18
    void* m_pRefData;           // 0x20
   private:
    PAD(0x8);

   public:
    matrix3x4_t m_transform;  // 0x30
   private:
    PAD(0x26);

   public:
    uint16_t m_nObjectTypeFlags;    // 0x86
    uint16_t m_nGameRenderCounter;  // 0x88
    uint8_t m_clr[3];               // 0x8a
   private:
    PAD(0x3);

   public:
    void* m_pPVS;        // 0x90
    void* m_pExtraData;  // 0x98
   private:
    PAD(0x10);

   public:
    void* m_pWorld;                          // 0xb0
    CHandle<C_BaseEntity> m_hExternalOwner;  // 0xb8
   private:
    PAD(0x14);
};  // Size: 0xd0
#pragma pack(pop)

#pragma pack(push, 1)
class CSceneAnimatableObject : public CSceneObject {
    CUtlVector<matrix3x4a_t> m_worldSpaceRenderBones;  // 0xd0
    CUtlVector<float> m_flexControllerWeights;         // 0xe8
    int m_nPerVertexBoneInfluenceCount;                // 0x100
   private:
    PAD(0xc);

   public:
    void* m_pProceduralBoneTransforms;  // 0x110
    // incomplete.
};  // Size: 0x118
#pragma pack(pop)

class CMeshDrawPrimitive_t {
   public:
    void SetShaderType(const char* shaderName);
    void SetMaterialFunction(const char* functionName, int value);

    uint32_t m_nSortKey;
    PAD(0x4);
    union {
        CMeshInstance* m_pMeshInstance;
        void* m_pPayload;
    };
    PAD(0x8);
    const CSceneAnimatableObject* m_pObject;
    const CMaterial2* m_pMaterial;
    const CSceneSystemStencilState* m_pStencilStateOverride;
    // int16_t m_nEnvironmentMapId;
    // int16_t m_nLightProbeVolumeId;
    // PAD(0x4);
    const CSceneSystemBakedLightingInfo* m_pBakedLightingInfo;
    uint16_t m_perInstanceBakedLightingParams[4];
    Color_t m_rgba;
    matrix3x4_t* m_pTransform;
    uint16_t m_nRequiredTextureSize;
    uint8_t m_nDrawCall;
    uint8_t m_nObjectClassSettings;
    uint16_t m_nBatchFlags;
    PAD(0x2);
};

class ISceneView {
   public:
    PAD(0xB98);
};

class ISceneLayer {
   public:
    void* vftable;
    PAD(0x6B8);
};

struct SceneSystemPerFrameStats_t {};

class CMaterial2 {
   public:
    virtual const char* GetName() const = 0;
    virtual const char* GetSharedName() const = 0;
};

struct MaterialKeyVar_t {
    MaterialKeyVar_t(uint64_t key, const char* name) : key(key), name(name) {}
    MaterialKeyVar_t(const char* name, bool findKey = false) : name(name) { key = findKey ? FindKey(name) : 0x0; }

    uint64_t key;
    const char* name;

    uint64_t FindKey(const char* name);
};

class CObjectInfo {
    PAD(0xB0);

   public:
    int id;
};

class CSkeletonInstance;

class CMaterialSystem {
   public:
    static CMaterialSystem* Get();

    CMaterial2*** FindOrCreateFromResource(CMaterial2*** out, const char* materialName);
    CMaterial2** CreateMaterial(void* out, const char* materialName, void* data, unsigned int unk, uint8_t unk2);
    void SetCreateDataByMaterial(CMaterial2*** const in, const void* data);
};
