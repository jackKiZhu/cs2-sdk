#pragma once

#include <bindings/baseflex.hpp>

class CCSPlayer_ViewModelServices;
class CCSPlayer_WeaponServices;
class C_CSWeaponBaseGun;

class C_BaseCombatCharacter : public C_BaseFlex {
   public:
    SCHEMA(CUtlVector<CHandle<C_BaseEntity>>, m_hMyWearables, "C_BaseCombatCharacter", "m_hMyWearables");
};

class C_BasePlayerPawn : public C_BaseCombatCharacter {
   public:
    SCHEMA(CCSPlayer_WeaponServices*, m_pWeaponServices, "C_BasePlayerPawn", "m_pWeaponServices");

    C_CSWeaponBaseGun* GetActiveWeapon();
};

class C_CSPlayerPawnBase : public C_BasePlayerPawn {
   public:
    SCHEMA(int, m_iShotsFired, "C_CSPlayerPawnBase", "m_iShotsFired");
    SCHEMA(int, m_ArmorValue, "C_CSPlayerPawnBase", "m_ArmorValue");
    SCHEMA(bool, m_bIsScoped, "C_CSPlayerPawnBase", "m_bIsScoped");
    SCHEMA(bool, m_bIsDefusing, "C_CSPlayerPawnBase", "m_bIsDefusing");
    SCHEMA(bool, m_bIsGrabbingHostage, "C_CSPlayerPawnBase", "m_bIsGrabbingHostage");
    SCHEMA(bool, m_bGunGameImmunity, "C_CSPlayerPawnBase", "m_bGunGameImmunity");
    SCHEMA(bool, m_bWaitForNoAttack, "C_CSPlayerPawnBase", "m_bWaitForNoAttack");
    SCHEMA(CCSPlayer_ViewModelServices*, m_pViewModelServices, "C_CSPlayerPawnBase", "m_pViewModelServices");

    bool IsPlayerPawn();
    bool IsObserverPawn();
    bool CanAttack(const float serverTime);
};

class C_CSPlayerPawn : public C_CSPlayerPawnBase {
   public:
    SCHEMA(CUtlVector<Vector>, m_aimPunchCache, "C_CSPlayerPawn", "m_aimPunchCache");
};
