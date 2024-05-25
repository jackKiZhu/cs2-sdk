#pragma once

#include <bindings/baseentity.hpp>

using CFiringModeFloat = float[2];
using CSkillFloat = float[4];

enum WeaponType : uint32_t {
    WEAPONTYPE_KNIFE = 0,
    WEAPONTYPE_PISTOL = 1,
    WEAPONTYPE_SUBMACHINEGUN = 2,
    WEAPONTYPE_RIFLE = 3,
    WEAPONTYPE_SHOTGUN = 4,
    WEAPONTYPE_SNIPER_RIFLE = 5,
    WEAPONTYPE_MACHINEGUN = 6,
    WEAPONTYPE_C4 = 7,
    WEAPONTYPE_TASER = 8,
    WEAPONTYPE_GRENADE = 9,
    WEAPONTYPE_EQUIPMENT = 10,
    WEAPONTYPE_STACKABLEITEM = 11,
    WEAPONTYPE_FISTS = 12,
    WEAPONTYPE_BREACHCHARGE = 13,
    WEAPONTYPE_BUMPMINE = 14,
    WEAPONTYPE_TABLET = 15,
    WEAPONTYPE_MELEE = 16,
    WEAPONTYPE_SHIELD = 17,
    WEAPONTYPE_ZONE_REPULSOR = 18,
    WEAPONTYPE_UNKNOWN = 19
};

class CBasePlayerVData
{
   public:

};

class CBasePlayerWeaponVData
{
   public:
    SCHEMA(bool, m_bIsReloading, "CBasePlayerWeaponVData", "m_bIsReloading");
    SCHEMA(int, m_iMaxClip1, "CBasePlayerWeaponVData", "m_iMaxClip1");
};

class CCSWeaponBaseVData : public CBasePlayerWeaponVData
{
   public:
    SCHEMA(int, m_WeaponType, "CCSWeaponBaseVData", "m_WeaponType");
    SCHEMA(float, m_flRange, "CCSWeaponBaseVData", "m_flRange");
    SCHEMA(CFiringModeFloat, m_flMaxSpeed, "CCSWeaponBaseVData", "m_flMaxSpeed");

    bool IsGun();

    PAD(0x10);
    const char* subclassID;
};
