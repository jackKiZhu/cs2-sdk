#include <pch.hpp>

#include <bindings/econitemview.hpp>
#include <interfaces/inventory.hpp>

CEconItem* C_EconItemView::GetSOCData() {
    CCSPlayerInventory* inventory = CCSPlayerInventory::Get();
    return inventory ? inventory->GetSOCDataForItem(this->m_iItemID()) : nullptr;
}
