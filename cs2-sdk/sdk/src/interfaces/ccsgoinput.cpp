#include "pch.hpp"

#include <math/types/vector.hpp>

#include <signatures/signatures.hpp>

#include <interfaces/cvar.hpp>
#include <interfaces/ccsgoinput.hpp>

CCSGOInput* CCSGOInput::Get() {
    static auto inst = signatures::GetCSGOInput.GetPtrAs<CCSGOInput*>();
    return inst;
}

CUserCmd* CCSGOInput::GetUserCmd() { return GetUserCmd(sequenceNumber); }

CUserCmd* CCSGOInput::GetUserCmd(uint32_t sequenceNumber) { return &commands[sequenceNumber]; }

void ValidateUserCmd(const Vector& original, CUserCmd* cmd, CCSGOInputHistoryEntryPB* entry) {
    if (!cmd || !cmd->baseCmd || !entry) return;

    static ConVar* m_pitch = CCVar::Get()->GetCvarByName("m_pitch");
    static ConVar* m_yaw = CCVar::Get()->GetCvarByName("m_yaw");
    static ConVar* sensitivity = CCVar::Get()->GetCvarByName("sensitivity");
    if (!m_pitch || !m_yaw || !sensitivity) return;

    const Vector delta = (entry->pViewCmd->viewangles - original).NormalizedAngle();
    if (delta.IsZero()) return;
    
    const float pitch = m_pitch->GetValue<float>();
    const float yaw = m_yaw->GetValue<float>();
    float s = sensitivity->GetValue<float>();
    if (s == 0.f) s = 1.f;

    cmd->baseCmd->mouseDx = static_cast<short>(delta.x / (s * pitch));
    cmd->baseCmd->mouseDy = static_cast<short>(-delta.y / (s * yaw));
}
