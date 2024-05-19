#include "pch.hpp"

#include <math/types/vector.hpp>

#include <signatures/signatures.hpp>

#include <interfaces/cvar.hpp>
#include <interfaces/ccsgoinput.hpp>

#include <virtual/virtual.hpp>

CCSGOInput* CCSGOInput::Get() {
    static auto inst = signatures::GetCSGOInput.GetPtrAs<CCSGOInput*>();
    return inst;
}

CUserCmd* CCSGOInput::GetUserCmd() { return sequenceNumber < 0 ? nullptr : GetUserCmd(sequenceNumber); }

CUserCmd* CCSGOInput::GetUserCmd(uint32_t sequenceNumber) { return &commands[sequenceNumber % 150]; }

size_t CBasePB::Checksum() { return vt::CallMethod<size_t>(this, 7); }

size_t CCSGOUserCmdPB::Checksum() { return baseCmd ? baseCmd->Checksum() : 0; }

char* CCSGOUserCmdPB::Checksum(char* a2, char* a3) { return vt::CallMethod<char*>(this, 10, a2, a3); }

void CUserCmd::SetBaseCmdButtons() { vt::CallMethod<void>(this, 15); }

void CUserCmd::GetBaseCmdButtons() { vt::CallMethod<void>(this, 16); }
