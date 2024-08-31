#include <pch.hpp>

#include <bindings/vdata.hpp>
#include <virtual/virtual.hpp>

bool CCSWeaponBaseVData::IsGun() { 
  const int weaponType = m_WeaponType();
    return weaponType >= WEAPONTYPE_PISTOL && weaponType <= WEAPONTYPE_MACHINEGUN;
}

void CCSWeaponBaseVData::Update() { vt::CallMethod<void>(this, 182); }

