#pragma once

#include <schemamgr/schema_manager.hpp>
#include <bindings/entityidentity.hpp>
#include <types/econitemdefinition.hpp>
#include <virtual/virtual.hpp>

class CEconItem;

class C_EconItemView {
   public:
    CEconItem* GetSOCData();

    int GetCustomPaintKitIndex() { return vt::CallMethod<int>(this, 2); }
    CEconItemDefinition* GetStaticData() { return vt::CallMethod<CEconItemDefinition*>(this, 13); }

    SCHEMA(uint16_t, m_iItemDefinitionIndex, "C_EconItemView", "m_iItemDefinitionIndex");
    SCHEMA(uint64_t, m_iItemID, "C_EconItemView", "m_iItemID");
    SCHEMA(uint32_t, m_iItemIDLow, "C_EconItemView", "m_iItemIDLow");
    SCHEMA(uint32_t, m_iItemIDHigh, "C_EconItemView", "m_iItemIDHigh");
    SCHEMA(uint32_t, m_iAccountID, "C_EconItemView", "m_iAccountID");
    SCHEMA(bool, m_bInitialized, "C_EconItemView", "m_bInitialized");
    SCHEMA(bool, m_bDisallowSOC, "C_EconItemView", "m_bDisallowSOC");
};
