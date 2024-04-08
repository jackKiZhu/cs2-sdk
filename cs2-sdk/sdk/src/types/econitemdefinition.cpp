#include <pch.hpp>

#include <types/econitemdefinition.hpp>
#include <fnv/fnv1a.hpp>

bool CEconItemDefinition::IsWeapon() { return GetStickersSupportedCount() >= 4; }

bool CEconItemDefinition::IsKnife(bool excludeDefault) {
    if (FNV1A::Hash(m_pszItemTypeName) != FNV1A::HashConst("#CSGO_Type_Knife")) return false;
    return excludeDefault ? m_nDefIndex >= 500 : true;
}

bool CEconItemDefinition::IsGlove(bool excludeDefault) { 
    if (FNV1A::Hash(m_pszItemTypeName) != FNV1A::HashConst("#Type_Hands")) return false;
    const bool defaultGlove = m_nDefIndex == 5028 || m_nDefIndex == 5029;
    return excludeDefault ? !defaultGlove : true;
}


