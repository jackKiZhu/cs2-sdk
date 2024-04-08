#include <pch.hpp>

#include <hacks/skinchanger/skinchanger.hpp>
#include <vars/vars.hpp>

#include <interfaces/engineclient.hpp>
#include <interfaces/inventory.hpp>
#include <interfaces/source2client.hpp>
#include <interfaces/econitemsystem.hpp>

#include <cache/cache.hpp>
#include <cache/entities/player.hpp>
#include <bindings/playercontroller.hpp>
#include <bindings/playerpawn.hpp>
#include <bindings/weapon.hpp>
#include <bindings/baseviewmodel.hpp>
#include <bindings/viewmodelservices.hpp>
#include <types/econitemschema.hpp>

bool CSkinChanger::IsEnabled() { return true; }

void CSkinChanger::OnFrameStageNotify(int stage) {
    if (stage != 6 || !CEngineClient::Get()->IsInGame()) return;

    CCSPlayerInventory* inventory = CCSPlayerInventory::Get();
    if (!inventory) return;

    CCachedPlayer* cachedLocal = CMatchCache::Get().GetLocalPlayer();
    if (!cachedLocal || !cachedLocal->IsValid()) return;

    CCSPlayerController* localController = cachedLocal->Get();
    if (!localController) return;

    C_CSPlayerPawnBase* localPawn = localController->m_hPawn().Get();
    if (!localPawn) return;

    CCSPlayer_ViewModelServices* viewModelServices = localPawn->m_pViewModelServices();
    if (!viewModelServices) return;

    CHandle<C_CSGOViewModel>* viewModels = viewModelServices->m_hViewModel();
    if (!viewModels) return;

    C_CSGOViewModel* viewModel = viewModels[0].Get();
    if (!viewModel) return;

    CCSPlayer_WeaponServices* weaponServices = localPawn->m_pWeaponServices();
    if (!weaponServices) return;

    const uint64_t steamID = inventory->GetOwner().id;

    auto weapons = weaponServices->m_hMyWeapons();
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

        C_EconItemView* loadoutItemView = nullptr;
        if (itemDefinition->IsWeapon()) {
            for (int i = 0; i <= 56; ++i) {
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

        const bool isKnife = loadoutItemDefinition->IsKnife(false);
        const bool isGlove = loadoutItemDefinition->IsGlove(false);
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
            if (viewModel->m_hWeapon() == weaponHandle) viewModel->SetModel(knifeModel);
            viewModel->animationGraphInstance->animGraphNetworkedVariables = nullptr;
        } else {
            // Use legacy weapon models only for skins that require them.
            // Probably need to cache this if you really care that much about
            // performance.

            std::optional<CPaintKit*> paintKit = CSource2Client::Get()->GetEconItemSystem()->GetEconItemSchema()->GetPaintKits().FindByKey(loadoutItemView->GetCustomPaintKitIndex());
            const bool usesOldModel = paintKit.has_value() && paintKit.value()->bUseLegacyModel;

            weaponSceneNode->SetMeshGroupMask(1 + usesOldModel);
            if (viewModel->m_hWeapon() == weaponHandle) {
                CGameSceneNode* viewmodelSceneNode = viewModel->m_pGameSceneNode();
                if (viewmodelSceneNode) 
                    viewmodelSceneNode->SetMeshGroupMask(1 + static_cast<int>(usesOldModel));
            }
        }
    }
}
