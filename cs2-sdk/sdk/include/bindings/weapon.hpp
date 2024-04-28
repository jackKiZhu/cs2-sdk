#pragma once

#include <bindings/econentity.hpp>

#include <signatures/signatures.hpp>
#include <virtual/virtual.hpp>

class C_BasePlayerWeapon : public C_EconEntity {
    public:
    SCHEMA(int, m_nNextPrimaryAttackTick, "C_BasePlayerWeapon", "m_nNextPrimaryAttackTick");
    SCHEMA(float, m_flNextPrimaryAttackTickRatio, "C_BasePlayerWeapon", "m_flNextPrimaryAttackTickRatio");
    SCHEMA(int, m_nNextSecondaryAttackTick, "C_BasePlayerWeapon", "m_nNextSecondaryAttackTick");
    SCHEMA(float, m_flNextSecondaryAttackTickRatio, "C_BasePlayerWeapon", "m_flNextSecondaryAttackTickRatio");
    SCHEMA(int, m_iClip1, "C_BasePlayerWeapon", "m_iClip1");
    SCHEMA(int, m_iClip2, "C_BasePlayerWeapon", "m_iClip2");

    bool CanFire() { return signatures::CanFire.GetPtrAs<bool (*)(C_BasePlayerWeapon*)>()(this); }
    float GetAccuracy() { return signatures::GetAccuracy.GetPtrAs<float (*)(C_BasePlayerWeapon*)>()(this); }

    float GetSpread() { return vt::CallMethod<float>(this, 351); }
    float GetInaccuracy() { return vt::CallMethod<float>(this, 397); }
};

class C_CSWeaponBase : public C_BasePlayerWeapon {
	public:
    SCHEMA(bool, m_bBurstMode, "C_CSWeaponBase", "m_bBurstMode");
    SCHEMA(bool, m_bInReload, "C_CSWeaponBase", "m_bInReload");
    SCHEMA(float, m_flPostponeFireReadyFrac, "C_CSWeaponBase", "m_flPostponeFireReadyFrac");
    SCHEMA(int, m_iBurstShotsRemaining, "C_CSWeaponBase", "m_iBurstShotsRemaining");
    SCHEMA(bool, m_bUIWeapon, "C_CSWeaponBase", "m_bUIWeapon");
    SCHEMA(int, m_iOriginalTeamNumber, "C_CSWeaponBase", "m_iOriginalTeamNumber");
    PSCHEMA_EXTENDED(void, m_hStattrakAttachment, CConstants::CLIENT_LIB, "C_CSWeaponBase", "m_iNumEmptyAttacks", 4);
    PSCHEMA_EXTENDED(void, m_hNametagAttachment, CConstants::CLIENT_LIB, "C_CSWeaponBase", "m_iNumEmptyAttacks", 20);
};

class C_CSWeaponBaseGun : public C_CSWeaponBase {
	public:
    SCHEMA(int, m_zoomLevel, "C_CSWeaponBaseGun", "m_zoomLevel");

    bool CanPrimaryAttack(const int weaponType, const float serverTime);
    bool CanSecondaryAttack(const int weaponType, const float serverTime);
};

class CPlayer_WeaponServices {
   public:
    SCHEMA(CUtlVector<CHandle<C_BasePlayerWeapon>>, m_hMyWeapons, "CPlayer_WeaponServices", "m_hMyWeapons");
    SCHEMA(CHandle<C_BasePlayerWeapon>, m_hActiveWeapon, "CPlayer_WeaponServices", "m_hActiveWeapon");
};

class CCSPlayer_WeaponServices : public CPlayer_WeaponServices {
   public:
	SCHEMA(float, m_flNextAttack, "CCSPlayer_WeaponServices", "m_flNextAttack");
};
