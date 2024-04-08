#include "pch.hpp"

#include <hacks/skinchanger/skinchanger.hpp>

//#include <bindings/playercontroller.hpp>
//#include <bindings/playerpawn.hpp>
//#include <cache/entities/player.hpp>
//#include <cache/cache.hpp>
//#include <bindings/entityinstance.hpp>

#include <vars/vars.hpp>

#include <interfaces/engineclient.hpp>
#include <interfaces/inventory.hpp>

bool CSkinChanger::IsEnabled() { return true; }

void CSkinChanger::OnFrameStageNotify(int stage) {
    if (stage != 6 || !CEngineClient::Get()->IsInGame()) return;

    CCSPlayerInventory* inventory = CCSPlayerInventory::Get();
    if (!inventory) return;
}
