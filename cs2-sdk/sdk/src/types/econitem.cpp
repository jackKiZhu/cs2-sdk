#include <pch.hpp>

#include <types/econitem.hpp>
#include <types/econitemschema.hpp>
#include <interfaces/econitemsystem.hpp>
#include <interfaces/source2client.hpp>

#include <signatures/signatures.hpp>
#include <virtual/virtual.hpp>

void CEconItem::SetDynamicAttributeValue(int index, void* value) {
    CEconItemSystem* econItemSystem = CSource2Client::Get()->GetEconItemSystem();
    if (!econItemSystem) return;

    CEconItemSchema* itemSchema = econItemSystem->GetEconItemSchema();
    if (!itemSchema) return;

    void* attributeDefinitionInterface = itemSchema->GetAttributeDefinitionInterface(index);
    if (!attributeDefinitionInterface) return;

    static auto SetDynamicAttributeValueUInt = signatures::SetDynamicAttributeValueUInt.GetPtrAs<void(*)(CEconItem*, void*, void*)>();
    if (!SetDynamicAttributeValueUInt) return;

    SetDynamicAttributeValueUInt(this, attributeDefinitionInterface, value);
}

void CEconItem::SetDynamicAttributeValueString(int index, const char* value) { return; }

CEconItem* CEconItem::CreateInstance() {
    auto Constuctor = signatures::CEconItem.GetPtrAs<CEconItem* (*)()>();
    return Constuctor ? Constuctor() : nullptr;
}

void CEconItem::Destruct(bool b) {
	vt::CallMethod<void>(this, 1, b);
}
