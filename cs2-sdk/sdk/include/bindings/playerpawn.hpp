#pragma once

#include <bindings/baseflex.hpp>
// #include <bindings/weapon.hpp>

class CPlayerWeaponServices;
class C_BasePlayerWeapon;

class C_BaseCombatCharacter : public C_BaseFlex {
   public:
    SCHEMA(CUtlVector<CHandle<C_BaseEntity>>, m_hMyWearables, "C_BaseCombatCharacter", "m_hMyWearables");
};

class C_BasePlayerPawn : public C_BaseCombatCharacter {
   public:
    SCHEMA(CPlayerWeaponServices*, m_pWeaponServices, "C_BasePlayerPawn", "m_pWeaponServices");

    C_BasePlayerWeapon* GetActiveWeapon();
};

class C_CSPlayerPawnBase : public C_BasePlayerPawn {
   public:
    SCHEMA(int, m_iShotsFired, "C_CSPlayerPawnBase", "m_iShotsFired");
    SCHEMA(bool, m_bIsScoped, "C_CSPlayerPawnBase", "m_bIsScoped");
    SCHEMA(bool, m_bIsDefusing, "C_CSPlayerPawnBase", "m_bIsDefusing");
    SCHEMA(bool, m_bGunGameImmunity, "C_CSPlayerPawnBase", "m_bGunGameImmunity");

    bool IsPlayerPawn();
    bool IsObserverPawn();
};

class C_CSPlayerPawn : public C_CSPlayerPawnBase {
   public:
    SCHEMA(CUtlVector<Vector>, m_aimPunchCache, "C_CSPlayerPawn", "m_aimPunchCache");
};
