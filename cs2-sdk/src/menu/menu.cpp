#include "pch.hpp"

#include <algorithm>

#include <menu/menu.hpp>
#include <logger/logger.hpp>

#include <vars/vars.hpp>

#include <instance/instance.hpp>
#include <renderer/renderer.hpp>

#include <interfaces/engineclient.hpp>
#include <interfaces/inputsystem.hpp>
#include <interfaces/source2client.hpp>
#include <interfaces/econitemsystem.hpp>
#include <interfaces/inventory.hpp>
#include <interfaces/localize.hpp>

#include <types/econitem.hpp>
#include <types/econitemschema.hpp>
#include <types/econitemdefinition.hpp>

#include <hacks/skinchanger/skinchanger.hpp>

#include <sdl/sdl.hpp>

#include <imgui/imgui.h>

void CMenu::Render() {
    if (ImGui::IsKeyPressed(ImGuiKey_Insert, false)) {
        Toggle(!IsOpen());
    } else if (ImGui::IsKeyPressed(ImGuiKey_End, false)) {
        return Shutdown(), CSkinChanger::Get().Shutdown(), CInstance::Get().FreeLibrary();
    }

    RenderWatermark();
    RenderUI();
}

void CMenu::Shutdown() {
    SDK_LOG_PROLOGUE();

    Toggle(false);
}

void CMenu::Toggle(bool state) {
    if (!ImGui::GetCurrentContext()) {
        return;
    }

    m_Open = state;

    auto inputSystem = CInputSystem::Get();
    if (inputSystem && inputSystem->IsRelativeMouseMode()) {
        const ImVec2 screenCenter = ImGui::GetIO().DisplaySize * 0.5f;

        sdl::SetRelativeMouseMode(!m_Open);
        sdl::SetWindowGrab(inputSystem->GetSDLWindow(), !m_Open);
        sdl::WarpMouseInWindow(nullptr, screenCenter.x, screenCenter.y);
    }
}

void CMenu::RenderWatermark() {
    auto drawList = CRenderer::GetBackgroundDrawList();

    char framerate[128];
    snprintf(framerate, IM_ARRAYSIZE(framerate), "Welcome [%d]\nFPS: %d\n\n%s %s",
             CEngineClient::Get()->GetEngineBuildNumber(), static_cast<int>(ImGui::GetIO().Framerate), __DATE__, __TIME__);

    drawList->AddText({17, 9}, IM_COL32(0, 0, 0, 255), framerate);
    drawList->AddText({16, 8}, IM_COL32(27, 227, 200, 255), framerate);
}

void CMenu::RenderUI() {
    ImGuiIO& IO = ImGui::GetIO();
    IO.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

    if (!IsOpen()) {
        return;
    }

    IO.ConfigFlags = ImGuiConfigFlags_None;

    sdl::SetRelativeMouseMode(false);

    RenderMainMenu();
    RenderInventoryWindow();
}

void CMenu::RenderMainMenu() {
    constexpr auto windowStartWidth = 256.f;

    const ImGuiIO& IO = ImGui::GetIO();

    ImGui::SetNextWindowSize({windowStartWidth, 0}, ImGuiCond_Once);
    ImGui::SetNextWindowPos(IO.DisplaySize * 0.5f, ImGuiCond_Once, {0.5f, 0.5f});
    ImGui::Begin("cs2-sdk | main menu", nullptr);
    {
        ImGui::Checkbox("Enable ESP", &g_Vars.m_EnableESP);
        ImGui::Checkbox("Enable If Spectating", &g_Vars.m_EnableIfSpectating);

        ImGui::SeparatorText("Visuals");

        ImGui::Checkbox("Players box", &g_Vars.m_PlayerBoxes);
        ImGui::Checkbox("Ignore team", &g_Vars.m_Team);
        ImGui::Checkbox("Players name", &g_Vars.m_PlayerNames);
        ImGui::Checkbox("Players healthbar", &g_Vars.m_PlayerHealthBar);

        ImGui::Checkbox("Weapons box", &g_Vars.m_WeaponBoxes);
        ImGui::Checkbox("Chickens box", &g_Vars.m_ChickenBoxes);
        ImGui::Checkbox("Hostages box", &g_Vars.m_HostageBoxes);
        ImGui::Checkbox("Others box", &g_Vars.m_OtherBoxes);

        ImGui::Checkbox("3D boxes", &g_Vars.m_Use3DBoxes);

        ImGui::SliderFloat("Recoil (X)", &g_Vars.m_RecoilX, 0.f, 1.f, "%.1f %%");
        ImGui::SliderFloat("Recoil (Y)", &g_Vars.m_RecoilY, 0.f, 1.f, "%.1f %%");

        ImGui::SeparatorText("Aimbot");
        ImGui::Checkbox("Aimbot", &g_Vars.m_EnableAimbot);
        ImGui::Checkbox("Enable duel", &g_Vars.m_EnableInDuel);
        ImGui::SliderFloat("Aimbot FOV", &g_Vars.m_AimFov, 0.f, 180.f);
        ImGui::SliderFloat("Delay FOV", &g_Vars.m_DelayFov, 0.f, 180.f);
        ImGui::SliderFloat("KP", &g_Vars.m_KP, 0.f, 1.f);
        ImGui::SliderFloat("KI", &g_Vars.m_KI, 0.f, 1.f);
        ImGui::SliderFloat("Damp", &g_Vars.m_Damp, 0.f, 1.f);
        ImGui::SliderFloat("Reaction", &g_Vars.m_ReactionTreshold, 0.f, 1.f);
        ImGui::SliderFloat("Bonus", &g_Vars.m_Bonus, 0.f, 1.f);

        ImGui::Checkbox("Draw FOV", &g_Vars.m_DrawFov);

        ImGui::Checkbox("Triggerbot", &g_Vars.m_EnableTriggerbot);
        ImGui::Spacing();

        ImGui::SliderFloat("World brightness", &g_Vars.m_NightMode, 0.f, 1.f);
        ImGui::SliderFloat("FOV", &g_Vars.m_Fov, 0.f, 20.f);
        ImGui::SliderFloat("Viewmodel FOV", &g_Vars.m_ViewmodelFov, 0.f, 20.f);
        ImGui::Checkbox("Chams", &g_Vars.m_Chams);
        ImGui::SameLine();
        static float clr[4];
        if (ImGui::ColorEdit4("##Chams color", clr, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
            g_Vars.m_ChamsColor = Color_t(clr[0], clr[1], clr[2], clr[3]);
        }

        if (ImGui::Button("Unload", {-FLT_MIN, 0})) Shutdown(), CSkinChanger::Get().Shutdown(), CInstance::Get().FreeLibrary();
    }
    ImGui::End();
}

void CMenu::RenderInventoryWindow() {
    static constexpr float windowWidth = 540.f;

    struct DumpedSkin_t {
        std::string m_name = "";
        int m_ID = 0;
        int m_rarity = 0;
    };
    struct DumpedItem_t {
        std::string m_name = "";
        uint16_t m_defIdx = 0;
        int m_rarity = 0;
        bool m_unusualItem = false;
        std::vector<DumpedSkin_t> m_dumpedSkins{};
        DumpedSkin_t* pSelectedSkin = nullptr;
    };
    static std::vector<DumpedItem_t> vecDumpedItems;
    static DumpedItem_t* pSelectedItem = nullptr;

    CEconItemSchema* pItemSchema = CSource2Client::Get()->GetEconItemSystem()->GetEconItemSchema();

    ImGui::Begin("cs2sdk item dumper", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (vecDumpedItems.empty() && ImGui::Button("Dump items", {windowWidth, 0})) {
        const CUtlMap<uint64_t, CEconItemDefinition*>& vecItems = pItemSchema->GetSortedItemDefinitionMap();
        const CUtlMap<int, CPaintKit*>& vecPaintKits = pItemSchema->GetPaintKits();
        const CUtlMap<uint64_t, AlternateIconData_t>& vecAlternateIcons = pItemSchema->GetAlternateIconsMap();

        for (const auto& it : vecItems) {
            CEconItemDefinition* pItem = it.m_value;
            if (!pItem) continue;

            const bool isWeapon = pItem->IsWeapon();
            const bool isKnife = pItem->IsKnife(true);
            const bool isGloves = pItem->IsGloves(true);
            const bool isAgent = pItem->IsAgent(true); 

            if (!isWeapon && !isKnife && !isGloves && !isAgent) 
                continue;
            
            // Some items don't have names.
            const char* itemBaseName = pItem->m_pszItemBaseName;
            if (!itemBaseName || itemBaseName[0] == '\0') continue;

            const uint16_t defIdx = pItem->m_nDefIndex;

            DumpedItem_t dumpedItem;
            dumpedItem.m_name = CLocalize::Get()->FindSafe(itemBaseName); 
            dumpedItem.m_defIdx = defIdx;
            dumpedItem.m_rarity = pItem->m_nItemRarity;

            if (isKnife || isGloves || isAgent) dumpedItem.m_unusualItem = true;

            // Add vanilla knives
            if (isKnife) {
                dumpedItem.m_dumpedSkins.emplace_back("Vanilla", 0, IR_ANCIENT);
            }                

            if (isAgent) {
                const std::string_view itemBaseName = pItem->m_pszItemBaseName;

                DumpedSkin_t dumpedSkin;
                dumpedSkin.m_name = CLocalize::Get()->FindSafe(itemBaseName.data());
                dumpedSkin.m_ID = -1;
                dumpedSkin.m_rarity = IR_ANCIENT;

                if (itemBaseName._Starts_with("#CSGO_CustomPlayer_t")) {
                    dumpedItem.m_dumpedSkins.emplace_back(dumpedSkin);
                } else if (itemBaseName._Starts_with("#CSGO_CustomPlayer_ct")) {
                    dumpedItem.m_dumpedSkins.emplace_back(dumpedSkin);
                } else {
                    CLogger::Log("Unknown agent: {}", itemBaseName.data());
                }
            } else {
                // We filter skins by guns.
                for (const auto& it : vecPaintKits) {
                    CPaintKit* pPaintKit = it.m_value;
                    if (!pPaintKit || pPaintKit->id == 0 || pPaintKit->id == 9001) continue;

                    const uint64_t skinKey = Helper_GetAlternateIconKeyForWeaponPaintWearItem(defIdx, (uint32_t)pPaintKit->id, 0);
                    if (vecAlternateIcons.FindByKey(skinKey).has_value()) {
                        DumpedSkin_t dumpedSkin;
                        dumpedSkin.m_name = CLocalize::Get()->FindSafe(pPaintKit->sDescriptionTag);
                        dumpedSkin.m_ID = (int)pPaintKit->id;
                        dumpedSkin.m_rarity = pPaintKit->nRarity;
                        dumpedItem.m_dumpedSkins.emplace_back(dumpedSkin);
                    }
                }
            }

            // Sort skins by rarity.
            if (!dumpedItem.m_dumpedSkins.empty() && isWeapon) {
                std::sort(dumpedItem.m_dumpedSkins.begin(), dumpedItem.m_dumpedSkins.end(),
                          [](const DumpedSkin_t& a, const DumpedSkin_t& b) { return a.m_rarity > b.m_rarity; });
            }

            vecDumpedItems.emplace_back(dumpedItem);
        }
    }

    if (!vecDumpedItems.empty()) {
        if (ImGui::Button("Add all items", {windowWidth, 0.f})) {
            for (const auto& item : vecDumpedItems) {
                for (const auto& skin : item.m_dumpedSkins) {
                    CCSPlayerInventory* pInventory = CCSPlayerInventory::Get();
                    CEconItem* pItem = CEconItem::CreateInstance();
                    if (pInventory && pItem) {
                        auto highestIDs = pInventory->GetHighestIDs();
                        pItem->m_ulID = highestIDs.first + 1;
                        pItem->m_unInventory = highestIDs.second + 1;
                        pItem->m_unAccountID = uint32_t(pInventory->GetOwner().id);
                        pItem->m_unDefIndex = item.m_defIdx;
                        if (item.m_unusualItem) pItem->m_nQuality = IQ_UNUSUAL;
                        pItem->m_nRarity = std::clamp(item.m_rarity + skin.m_rarity - 1, 0, (skin.m_rarity == 7) ? 7 : 6);

                        pItem->SetPaintKit((float)skin.m_ID);
                        pItem->SetPaintSeed(1.f);
                        if (pInventory->AddEconItem(pItem)) CSkinChanger::Get().AddEconItemToList(pItem);
                    }
                }
            }
        }

        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Will cause lag on weaker computers.");

        static ImGuiTextFilter itemFilter;
        itemFilter.Draw("##filter", windowWidth);

        if (ImGui::BeginListBox("##items", {windowWidth, 140.f})) {
            for (auto& item : vecDumpedItems) {
                if (!itemFilter.PassFilter(item.m_name.c_str())) continue;

                ImGui::PushID(&item);
                if (ImGui::Selectable(item.m_name.c_str(), pSelectedItem == &item)) {
                    if (pSelectedItem == &item)
                        pSelectedItem = nullptr;
                    else
                        pSelectedItem = &item;
                }
                ImGui::PopID();
            }
            ImGui::EndListBox();
        }

        if (pSelectedItem) {
            if (!pSelectedItem->m_dumpedSkins.empty()) {
                static ImGuiTextFilter skinFilter;
                skinFilter.Draw("##filter2", windowWidth);

                if (ImGui::BeginListBox("##skins", {windowWidth, 140.f})) {
                    for (auto& skin : pSelectedItem->m_dumpedSkins) {
                        if (!skinFilter.PassFilter(skin.m_name.c_str())) continue;

                        ImGui::PushID(&skin);
                        if (ImGui::Selectable(skin.m_name.c_str(), pSelectedItem->pSelectedSkin == &skin)) {
                            if (pSelectedItem->pSelectedSkin == &skin)
                                pSelectedItem->pSelectedSkin = nullptr;
                            else
                                pSelectedItem->pSelectedSkin = &skin;
                        }
                        ImGui::PopID();
                    }
                    ImGui::EndListBox();
                }
            }

            char buttonLabel[128];
            snprintf(buttonLabel, 128, "Add every %s skin", pSelectedItem->m_name.c_str());

            if (ImGui::Button(buttonLabel, {windowWidth, 0.f})) {
                for (const auto& skin : pSelectedItem->m_dumpedSkins) {
                    CEconItem* pItem = CEconItem::CreateInstance();
                    CCSPlayerInventory* pInventory = CCSPlayerInventory::Get();
                    if (pInventory && pItem) {
                        auto highestIDs = pInventory->GetHighestIDs();

                        pItem->m_ulID = highestIDs.first + 1;
                        pItem->m_unInventory = highestIDs.second + 1;
                        pItem->m_unAccountID = uint32_t(pInventory->GetOwner().id);
                        pItem->m_unDefIndex = pSelectedItem->m_defIdx;
                        if (pSelectedItem->m_unusualItem) pItem->m_nQuality = IQ_UNUSUAL;
                        pItem->m_nRarity = std::clamp(pSelectedItem->m_rarity + skin.m_rarity - 1, 0, (skin.m_rarity == 7) ? 7 : 6);

                        pItem->SetPaintKit((float)skin.m_ID);
                        pItem->SetPaintSeed(1.f);
                        if (pInventory->AddEconItem(pItem)) CSkinChanger::Get().AddEconItemToList(pItem);
                    }
                }
            }

            if (pSelectedItem->pSelectedSkin) {
                static float kitWear = 0.f;
                static int kitSeed = 1;
                static int gunKills = -1;
                static char gunName[32];

                bool vanillaSkin = pSelectedItem->pSelectedSkin->m_ID == 0;
                snprintf(buttonLabel, 128, "Add %s%s%s", pSelectedItem->m_name.c_str(), vanillaSkin ? "" : " | ",
                         vanillaSkin ? "" : pSelectedItem->pSelectedSkin->m_name.c_str());

                if (ImGui::Button(buttonLabel, {windowWidth, 0.f})) {
                    CEconItem* pItem = CEconItem::CreateInstance();
                    CCSPlayerInventory* pInventory = CCSPlayerInventory::Get();
                    if (pInventory && pItem) {
                        auto highestIDs = pInventory->GetHighestIDs();

                        pItem->m_ulID = highestIDs.first + 1;
                        pItem->m_unInventory = highestIDs.second + 1;
                        pItem->m_unAccountID = uint32_t(pInventory->GetOwner().id);
                        pItem->m_unDefIndex = pSelectedItem->m_defIdx;

                        if (pSelectedItem->m_unusualItem) pItem->m_nQuality = IQ_UNUSUAL;

                        // I don't know nor do care why the rarity is calculated
                        // like this. [Formula]
                        pItem->m_nRarity = std::clamp(pSelectedItem->m_rarity + pSelectedItem->pSelectedSkin->m_rarity - 1, 0,
                                                      (pSelectedItem->pSelectedSkin->m_rarity == 7) ? 7 : 6);

                        pItem->SetPaintKit((float)pSelectedItem->pSelectedSkin->m_ID);
                        pItem->SetPaintSeed((float)kitSeed);
                        pItem->SetPaintWear(kitWear);

                        if (gunKills >= 0) {
                            pItem->SetStatTrak(gunKills);
                            pItem->SetStatTrakType(0);

                            // Applied automatically on knives.
                            if (pItem->m_nQuality != IQ_UNUSUAL) pItem->m_nQuality = IQ_STRANGE;
                        }

                        if (gunName[0]) pItem->SetCustomName(gunName);

                        if (pInventory->AddEconItem(pItem)) CSkinChanger::Get().AddEconItemToList(pItem);

                        kitWear = 0.f;
                        kitSeed = 1;
                        gunKills = -1;
                        memset(gunName, '\0', IM_ARRAYSIZE(gunName));
                    }
                }

                ImGui::Dummy({0, 8});
                ImGui::SeparatorText("Extra settings");

                ImGui::TextUnformatted("Wear Rating");
                ImGui::SetNextItemWidth(windowWidth);
                ImGui::SliderFloat("##slider1", &kitWear, 0.f, 1.f, "%.9f", ImGuiSliderFlags_Logarithmic);

                ImGui::TextUnformatted("Pattern Template");
                ImGui::SetNextItemWidth(windowWidth);
                ImGui::SliderInt("##slider2", &kitSeed, 1, 1000);

                ImGui::TextUnformatted("StatTrak Count");
                ImGui::SetNextItemWidth(windowWidth);
                ImGui::SliderInt("##slider3", &gunKills, -1, INT_MAX / 2, gunKills == -1 ? "Not StatTrak" : "%d",
                                 ImGuiSliderFlags_Logarithmic);

                ImGui::TextUnformatted("Custom Name");
                ImGui::SetNextItemWidth(windowWidth);
                ImGui::InputTextWithHint("##input1", "Default", gunName, IM_ARRAYSIZE(gunName));
            }
        }
    }

    ImGui::End();
}
