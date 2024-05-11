#pragma once

#include <types/color.hpp>

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

class CSceneAnimatableObject {
    PAD(0xB0);

   public:
    int owner;
};

class CMeshData {
   public:
    void SetShaderType(const char* shaderName);
    void SetMaterialFunction(const char* functionName, int value);

    PAD(0x18);                                       // 0x0
    CSceneAnimatableObject* sceneAnimatableObject;   // 0x18
    CMaterial2* material;                            // 0x20
    PAD(0x18);                                       // 0x28
    Color_t color;                                   // 0x40
    PAD(0x4);                                        // 0x44
    CObjectInfo* objectInfo;                         // 0x48
};

class CMaterialSystem {
   public:
    static CMaterialSystem* Get();

    CMaterial2*** FindOrCreateFromResource(CMaterial2*** out, const char* materialName);
    CMaterial2** CreateMaterial(CMaterial2*** out, const char* materialName, CMeshData* data);
    void SetCreateDataByMaterial(CMaterial2*** const in, const void* data);
};
