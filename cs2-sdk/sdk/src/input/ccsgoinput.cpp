#include "pch.hpp"

#include <math/types/vector.hpp>
#include <input/ccsgoinput.hpp>

#include <signatures/signatures.hpp>

CCSGOInput* CCSGOInput::Get() {
    static auto inst = signatures::GetCSGOInput.GetPtrAs<CCSGOInput*>();
    return inst;
}

uint32_t CCSGOInput::GetSequenceNumber() { 
    return 0; 
}

CUserCmd* CCSGOInput::GetUserCmd() { return GetUserCmd(sequenceNumber); }

CUserCmd* CCSGOInput::GetUserCmd(uint32_t sequenceNumber) { return &commands[sequenceNumber]; }

