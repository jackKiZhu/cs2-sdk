#pragma once

#include <bindings/baseflex.hpp>

class C_CSWeaponBase;

enum ViewModelMaterial : uint32_t {
    VIEWMODEL_MATERIAL_GLOVES = 0xf143b82a,
    VIEWMODEL_MATERIAL_HOSTAGE = 0x1b52829c,
    VIEWMODEL_MATERIAL_EMPTY = 0xa6ebe9b9,
    VIEWMODEL_MATERIAL_SLEEVES = 0x423b2ed4,
    VIEWMODEL_MATERIAL_EMPTY2 = 0xc8d7255e
};

class CViewmodelMaterialRecord;

class CViewmodelMaterialInfo {
   public:
    CViewmodelMaterialRecord* records;
    uint32_t count;
};

class CViewmodelMaterialRecord {
   public:
    uint32_t _static; 
    uint32_t identifier; 
    uint32_t handle; 
    uint32_t typeIndex;
};

class C_BaseViewModel : public C_BaseModelEntity {
   public:
    SCHEMA(CHandle<C_CSWeaponBase>, m_hWeapon, "C_BaseViewModel", "m_hWeapon");
};

class CAnimGraphNetworkedVariables;

class CAnimationGraphInstance {
   public:
    char pad_0x0000[0x2E0]; // 0x0000
    CAnimGraphNetworkedVariables* animGraphNetworkedVariables;  // 0x02E0
};

class C_CSGOViewModel : public C_BaseViewModel {
   public:
    char pad_0x0000[0xD08]; // 0x0000
    CAnimationGraphInstance* animationGraphInstance;  // 0x0D08

    CViewmodelMaterialInfo* GetMaterialInfo();
    void InvalidateViewmodelMaterial();
};
