#pragma once

#include <interfaces/csgcclientsystem.hpp>
#include <types/gcclientsharedobjectcache.hpp>
#include <types/utlvector.hpp>

class CCSPlayerInventory;

class CCSInventoryManager {
   public:
    static CCSInventoryManager* Get();

    bool EquipItemInLoadout(int team, int slot, uint64_t itemID);
    CCSPlayerInventory* GetLocalInventory();
};

class CSharedObject;
class C_EconItemView;
class CEconItem;

class CCSPlayerInventory {
   public:
    static CCSPlayerInventory* Get();

    void SOCreated(SOID_t owner, CSharedObject* object, ESOCacheEvent _event);
    void SOUpdated(SOID_t owner, CSharedObject* object, ESOCacheEvent _event);
    void SODestroyed(SOID_t owner, CSharedObject* object, ESOCacheEvent _event);
    C_EconItemView* GetItemInLoadout(int _class, int slot);
    bool AddEconItem(CEconItem* item);
    bool RemoveEconItem(CEconItem* item);
    std::pair<uint64_t, uint32_t> GetHighestIDs();
    C_EconItemView* GetItemViewForItem(uint64_t itemID);
    CEconItem* GetSOCDataForItem(uint64_t itemID);

    SOID_t& GetOwner();
    CUtlVector<C_EconItemView*>& GetItemVector();
};
