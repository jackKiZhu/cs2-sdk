#include <pch.hpp>

#include <types/econitemschema.hpp>
#include <virtual/virtual.hpp>

class CEconItemDefinition;

void* CEconItemSchema::GetAttributeDefinitionInterface(int attribIndex) { return vt::CallMethod<void*>(this, 27, attribIndex); }

CUtlMap<uint64_t, CEconItemDefinition*>& CEconItemSchema::GetSortedItemDefinitionMap() {
    return CPointer(this).GetField<CUtlMap<uint64_t, CEconItemDefinition*>>(0x128);
}

CUtlMap<uint64_t, AlternateIconData_t>& CEconItemSchema::GetAlternateIconsMap() {
    return CPointer(this).GetField<CUtlMap<uint64_t, AlternateIconData_t>>(0x278);
}

CUtlMap<int, CPaintKit*>& CEconItemSchema::GetPaintKits() { 
    return CPointer(this).GetField<CUtlMap<int, CPaintKit*>>(0x2F0); 
}


