#include <pch.hpp>

#include <bindings/econentity.hpp>
#include <signatures/signatures.hpp>

CCSWeaponBaseVData* C_EconEntity::GetWeaponData() {
    C_AttributeContainer* attributes = m_AttributeManager();
    if (!attributes) return nullptr;
    C_EconItemView* itemView = attributes->m_Item();
    if (!itemView) return nullptr;
    static auto GetWeaponData = signatures::GetWeaponData.GetPtrAs<CCSWeaponBaseVData* (*)(C_EconItemView*)>();
    if (!GetWeaponData) return nullptr;
    return GetWeaponData(itemView);
}
