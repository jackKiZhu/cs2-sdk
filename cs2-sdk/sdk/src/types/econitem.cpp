#include <pch.hpp>

#include <types/econitem.hpp>

#include <signatures/signatures.hpp>
#include <virtual/virtual.hpp>

void CEconItem::SetDynamicAttributeValue(int index, void* value) {}

void CEconItem::SetDynamicAttributeValueString(int index, const char* value) {}

CEconItem* CEconItem::CreateInstance() {
    auto Constuctor = signatures::CEconItem.GetPtrAs<CEconItem* (*)()>();
    return Constuctor ? Constuctor() : nullptr;
}

void CEconItem::Destruct(bool b) {
	vt::CallMethod<void>(this, 1, b);
}
