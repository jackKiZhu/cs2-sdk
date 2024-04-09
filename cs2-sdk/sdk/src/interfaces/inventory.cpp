#include <pch.hpp>

#include <interfaces/inventory.hpp>
#include <interfaces/csgcclientsystem.hpp>
#include <types/econitem.hpp>
#include <types/gcclientsharedobjectcache.hpp>
#include <types/gcclientsharedobjecttypecache.hpp>
//#include <bindings/econentity.hpp>
#include <bindings/econitemview.hpp>

#include <signatures/signatures.hpp>
#include <virtual/virtual.hpp>

static CGCClientSharedObjectTypeCache* CreateBaseTypeCache(CCSPlayerInventory* inventory) {
    CSGCClientSystem* sgcClientSystem = CSGCClientSystem::Get();
    if (!sgcClientSystem) return nullptr;

    CSGCClient* sgcClient = sgcClientSystem->GetSGCClient();
    if (!sgcClient) return nullptr;

    CGCClientSharedObjectCache* SOCache = sgcClient->FindSOCache(&inventory->GetOwner());
    if (!SOCache) return nullptr;

    return SOCache->CreateBaseTypeCache(k_EEconTypeItem);
}

CCSInventoryManager* CCSInventoryManager::Get() { 
    static CCSInventoryManager* inventoryManager = signatures::GetInventoryManager.GetPtrAs<CCSInventoryManager*>();
    return inventoryManager;
}

bool CCSInventoryManager::EquipItemInLoadout(int team, int slot, uint64_t itemID) { 
    return vt::CallMethod<bool>(this, 54, team, slot, itemID);
}

CCSPlayerInventory* CCSInventoryManager::GetLocalInventory() { 
    return vt::CallMethod<CCSPlayerInventory*>(this, 57);
}

CCSPlayerInventory* CCSPlayerInventory::Get() { 
    CCSInventoryManager* inventoryManager = CCSInventoryManager::Get();
    return inventoryManager ? inventoryManager->GetLocalInventory() : nullptr;
}

void CCSPlayerInventory::SOCreated(SOID_t owner, CSharedObject* object, ESOCacheEvent _event) {
    return vt::CallMethod<void>(this, 0, owner, object, _event);
}

void CCSPlayerInventory::SOUpdated(SOID_t owner, CSharedObject* object, ESOCacheEvent _event) {
    return vt::CallMethod<void>(this, 1, owner, object, _event);
}

void CCSPlayerInventory::SODestroyed(SOID_t owner, CSharedObject* object, ESOCacheEvent _event) {
    return vt::CallMethod<void>(this, 2, owner, object, _event);
}

C_EconItemView* CCSPlayerInventory::GetItemInLoadout(int _class, int slot) { 
    return vt::CallMethod<C_EconItemView*>(this, 8, _class, slot);
}

bool CCSPlayerInventory::AddEconItem(CEconItem* item) {
    if (!item) return false;

    CGCClientSharedObjectTypeCache* SOTypeCache = ::CreateBaseTypeCache(this);
    if (!SOTypeCache || !SOTypeCache->AddObject((CSharedObject*)item)) return false;

    SOCreated(GetOwner(), (CSharedObject*)item, eSOCacheEvent_Incremental);
    return true;
}

bool CCSPlayerInventory::RemoveEconItem(CEconItem* item) { 
    if (!item) return false;

    CGCClientSharedObjectTypeCache* SOTypeCache = ::CreateBaseTypeCache(this);
    if (!SOTypeCache) return false;

    const CUtlVector<CEconItem*>& sharedObjects = SOTypeCache->GetVecObjects<CEconItem*>();
    if (!sharedObjects.Contains(item)) return false;

    SODestroyed(GetOwner(), (CSharedObject*)item, eSOCacheEvent_Incremental);
    SOTypeCache->RemoveObject((CSharedObject*)item);

    item->Destruct();
    return true;
}

std::pair<uint64_t, uint32_t> CCSPlayerInventory::GetHighestIDs() { 
    uint64_t maxItemID = 0;
    uint32_t maxInventoryID = 0;

    CGCClientSharedObjectTypeCache* SOTypeCache = ::CreateBaseTypeCache(this);
    if (SOTypeCache) {
        const CUtlVector<CEconItem*>& vecItems = SOTypeCache->GetVecObjects<CEconItem*>();

        for (CEconItem* it : vecItems) {
            if (!it) continue;

            // Checks if item is default.
            if ((it->m_ulID & 0xF000000000000000) != 0) continue;

            maxItemID = std::max(maxItemID, it->m_ulID);
            maxInventoryID = std::max(maxInventoryID, it->m_unInventory);
        }
    }

    return std::make_pair(maxItemID, maxInventoryID);
}

C_EconItemView* CCSPlayerInventory::GetItemViewForItem(uint64_t itemID) { 
    C_EconItemView* econItemView = nullptr;
    const CUtlVector<C_EconItemView*>& items = GetItemVector();
    for (C_EconItemView* it : items) {
        if (it && it->m_iItemID() == itemID) {
            econItemView = it;
            break;
        }
    }
    return econItemView;
}

CEconItem* CCSPlayerInventory::GetSOCDataForItem(uint64_t itemID) { 
    CEconItem* SOCData = nullptr;
    CGCClientSharedObjectTypeCache* SOTypeCache = ::CreateBaseTypeCache(this);
    if (SOTypeCache) {
        const CUtlVector<CEconItem*>& items = SOTypeCache->GetVecObjects<CEconItem*>();
        for (CEconItem* it : items) {
            if (it && it->m_ulID == itemID) {
                SOCData = it;
                break;
            }
        }
    }
    return SOCData;
}

SOID_t& CCSPlayerInventory::GetOwner() { return CPointer(this).GetField<SOID_t>(0x10); }

CUtlVector<C_EconItemView*>& CCSPlayerInventory::GetItemVector() { return CPointer(this).GetField<CUtlVector<C_EconItemView*>>(0x20); }
