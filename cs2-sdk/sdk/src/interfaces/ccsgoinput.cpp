#include "pch.hpp"

#include <math/types/vector.hpp>

#include <signatures/signatures.hpp>

#include <interfaces/cvar.hpp>
#include <interfaces/ccsgoinput.hpp>

CCSGOInput* CCSGOInput::Get() {
    static auto inst = signatures::GetCSGOInput.GetPtrAs<CCSGOInput*>();
    return inst;
}

CUserCmd* CCSGOInput::GetUserCmd() { return sequenceNumber < 0 ? nullptr : GetUserCmd(sequenceNumber); }

CUserCmd* CCSGOInput::GetUserCmd(uint32_t sequenceNumber) { return &commands[sequenceNumber % 150]; }
