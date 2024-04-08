#pragma once

#include <bindings/baseflex.hpp>

class C_CSWeaponBase;

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
};
