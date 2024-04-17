#pragma once

#include <bindings/baseflex.hpp>
#include <bindings/econitemview.hpp>
#include <bindings/vdata.hpp>

#include <signatures/signatures.hpp>

class C_AttributeContainer {
   public:
    PSCHEMA(C_EconItemView, m_Item, "C_AttributeContainer", "m_Item");
};

class C_EconEntity : public C_BaseFlex {
   public:
    PSCHEMA(C_AttributeContainer, m_AttributeManager, "C_EconEntity", "m_AttributeManager");
    SCHEMA(uint32_t, m_OriginalOwnerXuidLow, "C_EconEntity", "m_OriginalOwnerXuidLow");
    SCHEMA(uint32_t, m_OriginalOwnerXuidHigh, "C_EconEntity", "m_OriginalOwnerXuidHigh");
    uint64_t GetOriginalOwnerXuid() { return (uint64_t(m_OriginalOwnerXuidHigh()) << 32) | m_OriginalOwnerXuidLow(); }

    CCSWeaponBaseVData* GetWeaponData();
};
