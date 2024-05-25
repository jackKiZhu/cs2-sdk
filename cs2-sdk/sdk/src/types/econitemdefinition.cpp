#include <pch.hpp>

#include <types/econitemdefinition.hpp>
#include <hash/fnv1a.hpp>

bool CEconItemDefinition::IsWeapon() { return m_iStickerSlots >= 4; }

bool CEconItemDefinition::IsKnife(bool excludeDefault) {
    if (FNV1A::Hash(m_pszItemTypeName) != FNV1A::HashConst("#CSGO_Type_Knife")) return false;
    return excludeDefault ? m_nDefIndex >= 500 : true;
}

bool CEconItemDefinition::IsGloves(bool excludeDefault) { 
    if (FNV1A::Hash(m_pszItemTypeName) != FNV1A::HashConst("#Type_Hands")) return false;
    const bool defaultGlove = m_nDefIndex == 5028 || m_nDefIndex == 5029;
    return excludeDefault ? !defaultGlove : true;
}

bool CEconItemDefinition::IsAgent(bool excludeDefault) { 
    if (FNV1A::Hash(m_pszItemTypeName) != FNV1A::HashConst("#Type_CustomPlayer")) return false;
    const bool defaultGlove = m_nDefIndex == 5036 || m_nDefIndex == 5037;
    return excludeDefault ? !defaultGlove : true;
}

bool CEconItemDefinition::IsSticker() { return false; }

bool CEconItemDefinition::IsCase() { 
    if (FNV1A::Hash(m_pszItemTypeName) != FNV1A::HashConst("#CSGO_Type_WeaponCase")) return false;
    return true;
}


