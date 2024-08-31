#pragma once

#include <bindings/econitemview.hpp>
#include <bindings/econentity.hpp>

class CCSPlayer_ViewModelServices;
class CPlayer_CameraServices;
class CCSPlayer_WeaponServices;
class CCSPlayer_ObserverServices;
class C_CSWeaponBaseGun;

class CCSPlayer_ObserverServices {
   public:
    SCHEMA(uint8_t, m_iObserverMode, "CPlayer_ObserverServices", "m_iObserverMode");
    SCHEMA(CHandle<C_BaseEntity>, m_hObserverTarget, "CPlayer_ObserverServices", "m_hObserverTarget");
    SCHEMA(bool, m_bForcedObserverMode, "CPlayer_ObserverServices", "m_bForcedObserverMode");
};

class C_BaseCombatCharacter : public C_BaseFlex {
   public:
    SCHEMA(CUtlVector<CHandle<C_EconWearable>>, m_hMyWearables, "C_BaseCombatCharacter", "m_hMyWearables");
    SCHEMA(float, m_flFieldOfView, "C_BaseCombatCharacter", "m_flFieldOfView");
};

class C_BasePlayerPawn : public C_BaseCombatCharacter {
   public:
    SCHEMA(CCSPlayer_WeaponServices*, m_pWeaponServices, "C_BasePlayerPawn", "m_pWeaponServices");
    SCHEMA(CCSPlayer_ObserverServices*, m_pObserverServices, "C_BasePlayerPawn", "m_pObserverServices");
    SCHEMA(CPlayer_CameraServices*, m_pCameraServices, "C_BasePlayerPawn", "m_pCameraServices");

    C_CSWeaponBaseGun* GetActiveWeapon();
};

class C_CSPlayerPawnBase : public C_BasePlayerPawn {
   public:
    SCHEMA(int, m_ArmorValue, "C_CSPlayerPawnBase", "m_ArmorValue");
    SCHEMA(bool, m_bIsDefusing, "C_CSPlayerPawnBase", "m_bIsDefusing");
    SCHEMA(bool, m_bIsGrabbingHostage, "C_CSPlayerPawnBase", "m_bIsGrabbingHostage");
    SCHEMA(bool, m_bGunGameImmunity, "C_CSPlayerPawnBase", "m_bGunGameImmunity");
    SCHEMA(bool, m_bWaitForNoAttack, "C_CSPlayerPawnBase", "m_bWaitForNoAttack");
    SCHEMA(Vector, m_angEyeAngles, "C_CSPlayerPawnBase", "m_angEyeAngles");

    SCHEMA(CCSPlayer_ViewModelServices*, m_pViewModelServices, "C_CSPlayerPawnBase", "m_pViewModelServices");

    bool IsPlayerPawn();
    bool IsObserverPawn();
    bool CanAttack(const float serverTime);
};

class C_CSPlayerPawn : public C_CSPlayerPawnBase {
   public:
    SCHEMA(CUtlVector<Vector>, m_aimPunchCache, "C_CSPlayerPawn", "m_aimPunchCache");
    SCHEMA(bool, m_bNeedToReApplyGloves, "C_CSPlayerPawn", "m_bNeedToReApplyGloves");
    SCHEMA(C_EconItemView, m_EconGloves, "C_CSPlayerPawn", "m_EconGloves");
    SCHEMA(uint8_t, m_nEconGlovesChanged, "C_CSPlayerPawn", "m_nEconGlovesChanged");
    SCHEMA(int, m_iShotsFired, "C_CSPlayerPawn", "m_iShotsFired");
    SCHEMA(bool, m_bIsScoped, "C_CSPlayerPawn", "m_bIsScoped");

    uint32_t GetOwnerHandleIndex();
    Vector* GetLastAimPunch();
};
