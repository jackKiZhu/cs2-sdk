#pragma once

#include <bindings/baseflex.hpp>

#include <signatures/signatures.hpp>

class C_EconEntity : C_BaseFlex {
    public:
};

class C_BasePlayerWeapon : C_EconEntity {
    public:
    SCHEMA(int, m_nNextPrimaryAttackTick, "C_BasePlayerWeapon", "m_nNextPrimaryAttackTick");
    SCHEMA(float, m_flNextPrimaryAttackTickRatio, "C_BasePlayerWeapon", "m_flNextPrimaryAttackTickRatio");
    SCHEMA(int, m_iClip1, "C_BasePlayerWeapon", "m_iClip1");
    SCHEMA(int, m_iClip2, "C_BasePlayerWeapon", "m_iClip2");

    bool CanFire() { return signatures::CanFire.GetPtrAs<bool (*)(C_BasePlayerWeapon*)>()(this); }
};

class C_CSWeaponBase : public C_BasePlayerWeapon {
	public:
    SCHEMA(bool, m_bBurstMode, "C_CSWeaponBase", "m_bBurstMode");
    SCHEMA(bool, m_bInReload, "C_CSWeaponBase", "m_bInReload");
};

class C_CSWeaponBaseGun : public C_CSWeaponBase {
	public:
    SCHEMA(int, m_zoomLevel, "C_CSWeaponBaseGun", "m_zoomLevel");
};

class CPlayerWeaponServices {
   public:
    SCHEMA(CUtlVector<CHandle<C_BasePlayerWeapon>>, m_hMyWeapons, "CPlayer_WeaponServices", "m_hMyWeapons");
    SCHEMA(CHandle<C_BasePlayerWeapon>, m_hActiveWeapon, "CPlayer_WeaponServices", "m_hActiveWeapon");
};
