#include <pch.hpp>

#include <bindings/vdata.hpp>

bool CCSWeaponBaseVData::IsGun() { 
  const int weaponType = m_WeaponType();
    return weaponType >= WEAPONTYPE_PISTOL && weaponType <= WEAPONTYPE_MACHINEGUN;
}
