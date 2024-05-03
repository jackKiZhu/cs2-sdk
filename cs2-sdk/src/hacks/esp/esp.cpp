#include "pch.hpp"

#include <hacks/esp/esp.hpp>

#include <cache/cache.hpp>
#include <cache/entities/player.hpp>
#include <vars/vars.hpp>

#include <interfaces/engineclient.hpp>

#include <renderer/renderer.hpp>

#include <bindings/playerpawn.hpp>
#include <bindings/playercontroller.hpp>

bool CESP::IsEnabled() { 
  if (!CEngineClient::Get()->IsInGame()) return false;
  CCachedPlayer* player = CMatchCache::Get().GetLocalPlayer();
  if (!player) return false;
  CCSPlayerController* controller = player->Get();
  if (!controller) return false;
  C_CSPlayerPawn* pawn = controller->m_hPawn().Get();
  if (!pawn) return false;
  if (g_Vars.m_EnableIfSpectating && pawn->IsObserverPawn()) return true;
  return g_Vars.m_EnableESP; 
}

void CESP::Render() {
    const std::lock_guard<std::mutex> lock(CMatchCache::GetLock());

    const auto& cachedEntities = CMatchCache::GetCachedEntities();
    for (const auto& it : cachedEntities) {
        const auto& cachedEntity = it.second;
        if (!IsEnabled() || !cachedEntity->IsValid()) {
            cachedEntity->InvalidateDrawInfo();
            continue;
        }

        cachedEntity->DrawESP();
    }

    ImVec2 spectatorsPos = ImVec2(ImGui::GetIO().DisplaySize.x - 200, 10);
    if (ImDrawList* drawList = CRenderer::GetBackgroundDrawList(); drawList) {
        for (const auto& [name, mode] : CMatchCache::GetSpectators()) {
            drawList->AddText(spectatorsPos, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), std::format("{} - {}", name, mode).c_str());
            spectatorsPos.y += 10.f;
        }
    }
}

void CESP::Update() {
    const std::lock_guard<std::mutex> lock(CMatchCache::GetLock());

    const auto& cachedEntities = CMatchCache::GetCachedEntities();
    for (const auto& it : cachedEntities) {
        const auto& cachedEntity = it.second;
        if (!IsEnabled() || !cachedEntity->IsValid()) {
            cachedEntity->InvalidateDrawInfo();
            continue;
        }

        cachedEntity->CalculateDrawInfo();
    }

    CMatchCache::Get().UpdateSpectators();
}
