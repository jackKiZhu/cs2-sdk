#include "pch.hpp"

#include <hacks/movement/movement.hpp>

#include <bindings/playercontroller.hpp>
#include <bindings/playerpawn.hpp>
#include <cache/entities/player.hpp>
#include <cache/cache.hpp>
#include <bindings/entityinstance.hpp>

#include <vars/vars.hpp>

#include <interfaces/engineclient.hpp>

void CMovement::Run() {
    CCachedPlayer* cachedLocalPlayer = CMatchCache::Get().GetLocalPlayer();
    if (!cachedLocalPlayer) {
        return;
    }
    CCSPlayerController* controller = cachedLocalPlayer->Get();
    if (!controller || !controller->m_bPawnIsAlive()) {
        return;
    }

    C_CSPlayerPawnBase* pawn = controller->m_hPawn().Get();
    if (!pawn || pawn->IsObserverPawn()) {
        return;
    }
    if (!(pawn->m_pEntity()->m_flags() & 0x1)) {
        return;
	}

}
