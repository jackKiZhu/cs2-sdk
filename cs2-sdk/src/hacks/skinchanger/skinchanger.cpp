#include <pch.hpp>

#include <hacks/skinchanger/skinchanger.hpp>
#include <vars/vars.hpp>

#include <interfaces/engineclient.hpp>
#include <interfaces/inventory.hpp>
#include <interfaces/source2client.hpp>
#include <interfaces/econitemsystem.hpp>
#include <interfaces/cgameevent.hpp>
#include <interfaces/localize.hpp>

#include <cache/cache.hpp>
#include <cache/entities/player.hpp>
#include <bindings/playercontroller.hpp>
#include <bindings/playerpawn.hpp>
#include <bindings/weapon.hpp>
#include <bindings/baseviewmodel.hpp>
#include <bindings/viewmodelservices.hpp>
#include <types/econitemschema.hpp>
#include <types/econitem.hpp>

#include <fnv/fnv1a.hpp>
#include <logger/logger.hpp>

bool CSkinChanger::IsEnabled() { return true; }

void CSkinChanger::OnFrameStageNotify(int stage) {
    if (stage != 6 || !CEngineClient::Get()->IsInGame()) return;

    CCSPlayerInventory* inventory = CCSPlayerInventory::Get();
    if (!inventory) return;

    CCachedPlayer* cachedLocal = CMatchCache::Get().GetLocalPlayer();
    if (!cachedLocal || !cachedLocal->IsValid()) return;

    CCSPlayerController* localController = cachedLocal->Get();
    if (!localController) return;

    C_CSPlayerPawn* localPawn = localController->m_hPawn().Get();
    if (!localPawn) return;

    CCSPlayer_ViewModelServices* viewModelServices = localPawn->m_pViewModelServices();
    if (!viewModelServices) return;

    CHandle<C_CSGOViewModel>* viewModels = viewModelServices->m_hViewModel();
    if (!viewModels) return;

    C_CSGOViewModel* viewmodel = viewModels[0].Get();
    if (!viewmodel) return;

    CCSPlayer_WeaponServices* weaponServices = localPawn->m_pWeaponServices();
    if (!weaponServices) return;

    const uint64_t steamID = inventory->GetOwner().id;
    const auto& weapons = weaponServices->m_hMyWeapons();
    for (const auto& weaponHandle : weapons) {
        C_CSWeaponBase* weapon = static_cast<C_CSWeaponBase*>(weaponHandle.Get());
        if (!weapon) continue;
        if (weapon->GetOriginalOwnerXuid() != steamID) continue;

        C_AttributeContainer* attributes = weapon->m_AttributeManager();
        if (!attributes) continue;

        C_EconItemView* itemView = attributes->m_Item();
        if (!itemView) continue;

        CEconItemDefinition* itemDefinition = itemView->GetStaticData();
        if (!itemDefinition) continue;

        CGameSceneNode* weaponSceneNode = weapon->m_pGameSceneNode();
        if (!weaponSceneNode) continue;

        uint16_t idi = itemView->m_iItemDefinitionIndex();
        if (idi >= 43 && idi <= 49) continue;

        constexpr int INVENTORY_SLOTS = 57;

        C_EconItemView* loadoutItemView = nullptr;
        if (itemDefinition->IsWeapon()) {
            for (int i = 0; i < INVENTORY_SLOTS; ++i) {
                C_EconItemView* curItemView = inventory->GetItemInLoadout(weapon->m_iOriginalTeamNumber(), i);
                if (curItemView && curItemView->m_iItemDefinitionIndex() == itemDefinition->m_nDefIndex) {
                    loadoutItemView = curItemView;
                    break;
                }
            }
        } else {
            loadoutItemView = inventory->GetItemInLoadout(weapon->m_iOriginalTeamNumber(), itemDefinition->m_iLoadoutSlot);
        }

        if (!loadoutItemView) continue;
        if (!addedItemIDs.contains(loadoutItemView->m_iItemID())) continue;

        CEconItemDefinition* loadoutItemDefinition = loadoutItemView->GetStaticData();
        if (!loadoutItemDefinition) continue;

        const bool isKnife = loadoutItemDefinition->IsKnife();
        if (!isKnife && loadoutItemDefinition->m_nDefIndex != itemDefinition->m_nDefIndex) continue;

        itemView->m_bDisallowSOC() = false;
        itemView->m_iItemID() = loadoutItemView->m_iItemID();
        itemView->m_iItemIDHigh() = loadoutItemView->m_iItemIDHigh();
        itemView->m_iItemIDLow() = loadoutItemView->m_iItemIDLow();
        itemView->m_iAccountID() = uint32_t(steamID);

        if (!weapon->m_bUIWeapon()) {
            // TODO:
        }

        if (isKnife) {
            itemView->m_iItemDefinitionIndex() = loadoutItemDefinition->m_nDefIndex;
            const char* knifeModel = loadoutItemDefinition->m_pszBaseDisplayModel;
            
            weapon->SetModel(knifeModel);
            if (viewmodel->m_hWeapon() == weaponHandle) viewmodel->SetModel(knifeModel);
            viewmodel->animationGraphInstance->animGraphNetworkedVariables = nullptr;
        } 
        else {
            // Use legacy weapon models only for skins that require them.
            // Probably need to cache this if you really care that much about
            // performance.

            std::optional<CPaintKit*> paintKit = CSource2Client::Get()->GetEconItemSystem()->GetEconItemSchema()->GetPaintKits().FindByKey(loadoutItemView->GetCustomPaintKitIndex());
            const bool usesOldModel = paintKit.has_value() && paintKit.value()->bUseLegacyModel;

            weaponSceneNode->SetMeshGroupMask(1 + static_cast<int>(usesOldModel));
            if (viewmodel->m_hWeapon() == weaponHandle) {
                CGameSceneNode* viewmodelSceneNode = viewmodel->m_pGameSceneNode();
                if (viewmodelSceneNode) 
                    viewmodelSceneNode->SetMeshGroupMask(1 + static_cast<int>(usesOldModel));
            }
        }
    }

    const auto& wearables = localPawn->m_hMyWearables();
    for (const auto& wearableHandle : wearables) {
        C_EconWearable* wearable = wearableHandle.Get();
        if (!wearable) continue;

        C_AttributeContainer* attributes = wearable->m_AttributeManager();
        if (!attributes) continue;

        C_EconItemView* itemView = attributes->m_Item();
        if (!itemView) continue;

        CEconItemDefinition* itemDefinition = itemView->GetStaticData();
        if (!itemDefinition) continue;

        const std::string_view itemBaseName = itemDefinition->m_pszItemBaseName;
        CLogger::Log("{} - {} - {}", itemBaseName.data(), CLocalize::Get()->FindSafe(itemBaseName.data()), itemDefinition->m_nDefIndex);

        uint16_t idi = itemView->m_iItemDefinitionIndex();
        if (idi >= 43 && idi <= 49) continue;

        constexpr int INVENTORY_SLOTS = 57;

        C_EconItemView* loadoutItemView = inventory->GetItemInLoadout(localPawn->m_iTeamNum(), itemDefinition->m_iLoadoutSlot);
        if (!loadoutItemView) continue;

        if (!addedItemIDs.contains(loadoutItemView->m_iItemID())) continue;

        CEconItemDefinition* loadoutItemDefinition = loadoutItemView->GetStaticData();
        if (!loadoutItemDefinition) continue;

        const bool isAgent = loadoutItemDefinition->IsAgent();
        if (!isAgent && loadoutItemDefinition->m_nDefIndex != itemDefinition->m_nDefIndex) continue;

        itemView->m_bDisallowSOC() = false;
        itemView->m_iItemID() = loadoutItemView->m_iItemID();
        itemView->m_iItemIDHigh() = loadoutItemView->m_iItemIDHigh();
        itemView->m_iItemIDLow() = loadoutItemView->m_iItemIDLow();
        itemView->m_iAccountID() = uint32_t(steamID);

        if (isAgent) {
            itemView->m_iItemDefinitionIndex() = loadoutItemDefinition->m_nDefIndex;
            const char* agentModel = loadoutItemDefinition->m_pszBaseDisplayModel;

            localPawn->SetModel(agentModel);
            //if (viewmodel->m_hWeapon() == weaponHandle) viewmodel->SetModel(knifeModel);
            //viewmodel->animationGraphInstance->animGraphNetworkedVariables = nullptr;
        }
    }

    // gloves
    [&]() {
        static int glovesUpdateFrames = 0;
        C_EconItemView& glovesView = localPawn->m_EconGloves();
        CEconItemDefinition* glovesDefinition = glovesView.GetStaticData();
        if (!glovesDefinition) return;

        C_EconItemView* loadoutGlovesView = inventory->GetItemInLoadout(localController->m_iTeamNum(), LOADOUT_SLOT_CLOTHING_HANDS);
        if (!loadoutGlovesView) return;
        if (!addedItemIDs.contains(loadoutGlovesView->m_iItemID())) return;

        CEconItemDefinition* loadoutGlovesDefinition = loadoutGlovesView->GetStaticData();
        if (!loadoutGlovesDefinition || !loadoutGlovesDefinition->IsGloves()) return;

        CViewmodelMaterialInfo* viewmodelMaterialInfo = viewmodel->GetMaterialInfo();
        if (!viewmodelMaterialInfo || viewmodelMaterialInfo->count == 0) return;

        CViewmodelMaterialRecord* currentRecord = viewmodelMaterialInfo->GetRecord(VIEWMODEL_MATERIAL_GLOVES);
        if (!currentRecord) return;

        if (currentRecord->handle != glovesHandles[2] && glovesHandles[2] == glovesHandles[1] && glovesHandles[1] == glovesHandles[0])
            glovesUpdateFrames = 3;

        // shift each handle
        for (int i = 0; i < 2; i++) glovesHandles[i] = glovesHandles[i + 1];
        glovesHandles[2] = currentRecord->handle;

        if (glovesView.m_iItemID() != loadoutGlovesView->m_iItemID()) {
            glovesUpdateFrames = 3;
            glovesView.m_iItemDefinitionIndex() = loadoutGlovesView->m_iItemDefinitionIndex();
            glovesView.m_iItemID() = loadoutGlovesView->m_iItemID();
            glovesView.m_iItemIDHigh() = loadoutGlovesView->m_iItemIDHigh();
            glovesView.m_iItemIDLow() = loadoutGlovesView->m_iItemIDLow();
            glovesView.m_iAccountID() = uint32_t(steamID);
        }

        if (glovesUpdateFrames > 0) {
            CLogger::Log("i: {} | handle: {:#x} | typeidx: {} | static: {}", glovesUpdateFrames, currentRecord->handle, currentRecord->typeIndex, currentRecord->_static);
            glovesView.m_bInitialized() = true;
            localPawn->m_bNeedToReApplyGloves() = true;
            viewmodel->InvalidateViewmodelMaterial();
            glovesUpdateFrames--;
        }
    }();

    // agent
    [&]() {
        C_EconItemView* loadoutAgentView = inventory->GetItemInLoadout(localController->m_iTeamNum(), LOADOUT_SLOT_CLOTHING_CUSTOMPLAYER);
        if (!loadoutAgentView) return;
        if (!addedItemIDs.contains(loadoutAgentView->m_iItemID())) return;

        CEconItemDefinition* loadoutGlovesDefinition = loadoutAgentView->GetStaticData();
        if (!loadoutGlovesDefinition || !loadoutGlovesDefinition->IsAgent(true)) return;

        if (loadoutGlovesDefinition->m_pszBaseDisplayModel)
          localPawn->SetModel(loadoutGlovesDefinition->m_pszBaseDisplayModel);
    }();
}

void CSkinChanger::OnPreFireEvent(CGameEvent* _event) {
    if (!_event) return;
    const char* eventName = _event->GetName();
    if (!eventName) return;
    if (FNV1A::Hash(eventName) != FNV1A::HashConst("player_death")) return;
    
    CCachedPlayer* localPlayer = CMatchCache::Get().GetLocalPlayer();
    if (!localPlayer) return;
    CCSPlayerController* localController = localPlayer->Get();
    if (!localController) return;
    C_CSPlayerPawn* localPawn = localController->m_hPawn().Get();
    if (!localPawn) return;
    C_CSWeaponBaseGun* weapon = localPawn->GetActiveWeapon();
    if (!weapon) return;
    C_AttributeContainer* attributes = weapon->m_AttributeManager();
    if (!attributes) return;
    C_EconItemView* itemView = attributes->m_Item();
    if (!itemView) return;
    CEconItemDefinition* itemDefinition = itemView->GetStaticData();
    if (!itemDefinition || !itemDefinition->IsKnife(true)) return;
    
    CCSPlayerController* attackerController = _event->GetPlayerController("attacker");
    CCSPlayerController* victimController = _event->GetPlayerController("userid");
    if (attackerController == victimController) return;
    if (attackerController != localController) return;

    // _event->GetString("weapon") = itemDefinition->m_pszDefinitionName;

    // pEvent->SetString("weapon", pWeaponDefinition->GetSimpleWeaponName());
}

void CSkinChanger::OnEquipItemInLoadout(int team, int slot, uint64_t itemID) { 
    if (!addedItemIDs.count(itemID)) return;
    CCSInventoryManager* inventoryManager = CCSInventoryManager::Get();
    if (!inventoryManager) return;
    CCSPlayerInventory* inventory = inventoryManager->GetLocalInventory();
    if (!inventory) return;
    C_EconItemView* toEquipView = inventory->GetItemViewForItem(itemID);
    if (!toEquipView) return;
    C_EconItemView* currentView = inventory->GetItemInLoadout(team, slot);
    if (!currentView) return;
    CEconItemDefinition* currentDefinition = currentView->GetStaticData();
    if (!currentDefinition) return;

    CLogger::Log("Equipping {}", CLocalize::Get()->FindSafe(currentDefinition->m_pszBaseDisplayModel));

    if (currentDefinition->IsGloves(false) || currentDefinition->IsKnife(false) ||
        currentDefinition->m_nDefIndex == toEquipView->m_iItemDefinitionIndex())
        return;

    const uint64_t defaultItemID = (std::uint64_t(0xF) << 60) | toEquipView->m_iItemDefinitionIndex();
    inventoryManager->EquipItemInLoadout(team, slot, defaultItemID);

    CEconItem* curSOCData = currentView->GetSOCData();
    if (!curSOCData) return;

    inventory->SOUpdated(inventory->GetOwner(), (CSharedObject*)curSOCData, eSOCacheEvent_Incremental);
}

void CSkinChanger::OnSetModel(C_BaseModelEntity* entity, const char*& model) { 
    if (!entity || !entity->IsViewmodelV()) return;

    C_BaseViewModel* viewModel = static_cast<C_BaseViewModel*>(entity);
    if (!viewModel) return;

    CCSPlayerInventory* inventory = CCSPlayerInventory::Get();
    if (!inventory) return;

    const uint64_t steamID = inventory->GetOwner().id;

    C_CSWeaponBase* weapon = viewModel->m_hWeapon().Get();
    if (!weapon || !weapon->IsWeapon() || weapon->GetOriginalOwnerXuid() != steamID) return;

    C_AttributeContainer* attributes = weapon->m_AttributeManager();
    if (!attributes) return;

    C_EconItemView* itemView = attributes->m_Item();
    if (!itemView) return;

    CEconItemDefinition* itemDefinition = itemView->GetStaticData();
    if (!itemDefinition) return;

    C_EconItemView* loadoutItemView = inventory->GetItemInLoadout(weapon->m_iOriginalTeamNumber(), itemDefinition->m_iLoadoutSlot);
    if (!loadoutItemView) return;
    if (!addedItemIDs.contains(loadoutItemView->m_iItemID())) return;

    CEconItemDefinition* loadoutItemDefinition = loadoutItemView->GetStaticData();
    if (!loadoutItemDefinition || !loadoutItemDefinition->IsKnife(true)) return;

    model = loadoutItemDefinition->m_pszBaseDisplayModel;
}

void CSkinChanger::AddEconItemToList(CEconItem* item) { addedItemIDs.insert(item->m_ulID); }

void CSkinChanger::Shutdown() {
    CCSPlayerInventory* inventory = CCSPlayerInventory::Get();
    if (!inventory) return;
    for (uint64_t itemID : addedItemIDs) 
        inventory->RemoveEconItem(inventory->GetSOCDataForItem(itemID));
    addedItemIDs.clear();
}
