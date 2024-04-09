#include <pch.hpp>

#include <types/econitemschema.hpp>
#include <signatures/signatures.hpp>
#include <virtual/virtual.hpp>

class CEconItemDefinition;

void* CEconItemSchema::GetAttributeDefinitionInterface(int attribIndex) { return vt::CallMethod<void*>(this, 27, attribIndex); }

CUtlMap<uint64_t, CEconItemDefinition*>& CEconItemSchema::GetSortedItemDefinitionMap() {
    return CPointer(this).GetField<CUtlMap<uint64_t, CEconItemDefinition*>>(uint32_t(signatures::GetSortedItemDefinitionMap.GetPtr().Get()) -
                                                                            0x8);
}

CUtlMap<uint64_t, AlternateIconData_t>& CEconItemSchema::GetAlternateIconsMap() {
    return CPointer(this).GetField<CUtlMap<uint64_t, AlternateIconData_t>>(uint32_t(signatures::GetAlternateIconsMap.GetPtr().Get()) - 0x8);
}

CUtlMap<int, CPaintKit*>& CEconItemSchema::GetPaintKits() { 
    return CPointer(this).GetField<CUtlMap<int, CPaintKit*>>(uint32_t(signatures::GetPaintKits.GetPtr().Get()) - 0x8);
}


