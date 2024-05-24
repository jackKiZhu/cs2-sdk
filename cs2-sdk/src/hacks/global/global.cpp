#include <pch.hpp>
#include <hacks/global/global.hpp>
#include <interfaces/ccsgoinput.hpp>
#include <interfaces/engineclient.hpp>

bool CGlobal::Update(CUserCmd* cmd) {
    controller = nullptr;
    pawn = nullptr;
    weapon = nullptr;
    vdata = nullptr;

    this->cmd = cmd;

    if (!CEngineClient::Get()->IsInGame()) return false;
    if (!cmd) return false;
    player = CMatchCache::Get().GetLocalPlayer();
    if (!player) return false;
    controller = player->Get();
    if (!controller) return false;
    pawn = controller->m_hPawn().Get();
    if (!pawn) return false;
    weapon = pawn->GetActiveWeapon();
    if (!weapon) return false;
    vdata = weapon->GetWeaponData();
    if (!vdata) return false;

    Vector* punch = pawn->GetLastAimPunch();
    if (punch) {
        punchDelta = (*punch - aimPunch).NormalizedAngle();
        aimPunch = *punch;
    }
    pawn->GetEyePos(&eyePos);

    const CUtlVector<CMoveData>& moves = CCSGOInput::Get()->moves;
    if (moves.m_Size > 0) {
        CMoveData& lastMove = *CCSGOInput::Get()->moves.AtPtr(CCSGOInput::Get()->moves.m_Size - 1);
        viewAngles = lastMove.viewAngles;
        rcsAngles = lastMove.viewAngles - punchDelta * 2.f;
        rcsAngles.z = 0.f;
        rcsAngles.NormalizeAngle();
    }
    return true;
}
