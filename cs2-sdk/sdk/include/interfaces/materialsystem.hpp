#pragma once

#include <types/color.hpp>
#include <bindings/baseentity.hpp>

class ISceneObjectDesc
{
   public:
    void* vftable;
};

class CRenderAttributes
{
   public:
    PAD(0x320);
};

class IRenderContext
{
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

#pragma pack(1)
class CSceneObject {
   public:
    void* vftable;
    union {
        CMeshInstance* m_pMeshInstanceData;
        void* m_pObjectData;
    };
    ISceneObjectDesc* m_pDesc;
    void* m_pRefData;
    float m_flStartFadeDistanceSquared;
    float m_flFarCullDistanceSquared;
    uint16_t m_nObjectTypeFlags;
    uint16_t m_nGameRenderCounter;
    uint8_t m_tint[3];
    uint8_t m_nMeshGroupMaskSmall;
    uint8_t m_nDebugLevel : 2;
    uint8_t m_nSizeCullBloat : 2;
    uint8_t m_nBoundsType : 1;
    uint8_t m_nID;
    uint8_t m_nNumTransformBlocks;
    uint8_t m_nObjectClass;
    PAD(0xC);
    matrix3x4a_t m_transform;
    void* m_pPVS;
    void* m_pExtraData;
    uint64_t m_nOriginalRenderableFlags;
    uint64_t m_nRenderableFlags;
    void* m_pWorld;
    uint32_t m_hExternalOwner;
    CUtlStringToken m_nLayerMatchID;
};
#pragma pop(pack);

class CSceneAnimatableObject : public CSceneObject {
    PAD(0xC);

   public:
    CHandle<C_BaseEntity> ownerHandle;

   private:
    PAD(0x40);
};

class CMeshDrawPrimitive_t
{
   public:
    void SetShaderType(const char* shaderName);
    void SetMaterialFunction(const char* functionName, int value);

    uint32_t m_nSortKey;
    PAD(0x4);
    union {
        CMeshInstance* m_pMeshInstance;
        void* m_pPayload;
    };
    const CSceneAnimatableObject* m_pObject;
    const CMaterial2* m_pMaterial;
    const CSceneSystemStencilState* m_pStencilStateOverride;
    int16_t m_nEnvironmentMapId;
    int16_t m_nLightProbeVolumeId;
    PAD(0x4);
    const CSceneSystemBakedLightingInfo* m_pBakedLightingInfo;
    uint16_t m_perInstanceBakedLightingParams[4];
    Color_t m_rgba;
    const matrix3x4_t* m_pTransform;
    uint16_t m_nRequiredTextureSize;
    uint8_t m_nDrawCall;
    uint8_t m_nObjectClassSettings;
    uint16_t m_nBatchFlags;
    PAD(0x2);
};

class ISceneView
{
   public:
    PAD(0xB98);
};

class ISceneLayer
{
   public:
    void* vftable;
    PAD(0x6B8);
};

struct SceneSystemPerFrameStats_t
{

};

class CMaterial2 {
   public:
    virtual const char* GetName() = 0;
    virtual const char* GetSharedName() = 0;
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
