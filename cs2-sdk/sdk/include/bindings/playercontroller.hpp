#pragma once

#include <bindings/baseentity.hpp>

class C_CSPlayerPawnBase;
class C_CSPlayerPawn;
class C_CSWeaponBaseGun;

class CBasePlayerController : public C_BaseEntity {
   public:
    SCHEMA(uint32_t, m_nTickBase, "CBasePlayerController", "m_nTickBase");
    SCHEMA(CHandle<C_CSPlayerPawn>, m_hPawn, "CBasePlayerController", "m_hPawn");
    SCHEMA(CHandle<C_CSPlayerPawn>, m_hPredictedPawn, "CBasePlayerController", "m_hPredictedPawn");
    SCHEMA(bool, m_bIsLocalPlayerController, "CBasePlayerController", "m_bIsLocalPlayerController");
};

class CCSPlayerController : public CBasePlayerController {
   public:
    SCHEMA(bool, m_bPawnIsAlive, "CCSPlayerController", "m_bPawnIsAlive");
    SCHEMA(const char*, m_sSanitizedPlayerName, "CCSPlayerController", "m_sSanitizedPlayerName");
};
