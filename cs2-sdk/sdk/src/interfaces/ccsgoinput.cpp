#include "pch.hpp"

#include <math/types/vector.hpp>
#include <types/utlbuffer.hpp>

#include <signatures/signatures.hpp>

#include <interfaces/cvar.hpp>
#include <interfaces/ccsgoinput.hpp>

#include <virtual/virtual.hpp>

#include <logger/logger.hpp>

#include <memory/memory.hpp>
#include <constants/constants.hpp>

CCSGOInput* CCSGOInput::Get() {
    static auto inst = signatures::GetCSGOInput.GetPtrAs<CCSGOInput*>();
    return inst;
}

void CCSGOInput::Scroll(uint64_t button) {
    CMoveData* move = GetMoveData(0);
    if (!move) return;

    EInButtonState state = GetButtonState(&move->buttonsHeld, button);

    float when = 0.9f;

    CLogger::Log("+ button({}) {}: held:{} pressed:{} released:{} prev:{} -> {}", button, sequenceNumber, move->buttonsHeld & button,
                 move->buttonsPressed & button, move->buttonsReleased & button, move->prevButtonsHeld & button, (uintptr_t)state);

    if (state == IN_BUTTON_IDLE) {
        //move->Push(button, true, when);
        //move->Push(button, false, when);

        move->buttonsHeld &= ~button;
        move->prevButtonsHeld &= ~button;
        move->buttonsPressed |= button;
        move->buttonsReleased |= button;
    } else if (state == IN_BUTTON_RELEASED) {
        //move->Pop(button);
        move->buttonsHeld &= ~button;
        //move->prevButtonsHeld &= ~button;
        //move->Push(button, true, when);
        //move->Push(button, false, when);

        //move->prevButtonsHeld &= ~button;
        //move->buttonsPressed |= button;
    } else if (state == IN_BUTTON_HELD) {
        //move->Pop(button);
        //move->Push(button, false, when);
        move->buttonsHeld &= ~button;
        move->buttonsReleased |= button;
    } else if (state == IN_BUTTON_HELD_SCROLL) {
        //move->Pop(button);
        move->buttonsHeld &= ~button;
        move->prevButtonsHeld &= ~button;
        move->buttonsPressed &= ~button;
        move->buttonsReleased &= ~button;
    } else if (state == IN_BUTTON_SCROLL) {
        move->Pop(button);
        move->buttonsHeld &= ~button;
        move->prevButtonsHeld &= ~button;
        move->buttonsPressed &= ~button;
        move->buttonsReleased &= ~button;
    }

    CLogger::Log("- button({}) {}: held:{} pressed:{} released:{} prev:{} -> {}", button, sequenceNumber, move->buttonsHeld & button,
                 move->buttonsPressed & button, move->buttonsReleased & button, move->prevButtonsHeld & button, (uintptr_t)GetButtonState(&move->buttonsHeld, button));
}

void CCSGOInput::Release(uint64_t button) {
    CMoveData* move = GetMoveData(0);
    if (!move) return;

    bool wasPressed = IsButtonPressed(&move->buttonsHeld, button);
    // strip all current input attempted
    uint8_t removed = move->Pop(button);

    move->buttonsReleased &= ~button;
    move->buttonsHeld &= ~button;
    move->buttonsPressed &= ~button;

    // clear stored pressed state
    if (wasPressed) {
        move->buttonsPressed |= button;
        move->Push(button, false);
    }

    CLogger::Log("Release {} (pressed: {})", button, wasPressed);
}

// 4C 63 C2 4B 8D 04 40
CMoveData* CCSGOInput::GetMoveData(int index) {
    CCSGOInput* input = (CCSGOInput*)((uintptr_t)this + 24 * index);
    int size = input->moves.m_Size;
    if (size > 0) return input->moves.AtPtr(size - 1);
    return (CMoveData*)((uintptr_t)this + 0x5260 * index + 0x52C0);
}

CUserCmd* CCSGOInput::GetUserCmd() { return sequenceNumber < 0 ? nullptr : GetUserCmd(sequenceNumber); }

CUserCmd* CCSGOInput::GetUserCmd(uint32_t sequenceNumber) { return &commands[sequenceNumber % 150]; }

size_t CBasePB::Checksum() { return vt::CallMethod<size_t>(this, 7); }

size_t CCSGOUserCmdPB::Checksum() { return baseCmd ? baseCmd->Checksum() : 0; }

char* CCSGOUserCmdPB::Checksum(char* a2, char* a3) { return vt::CallMethod<char*>(this, 10, a2, a3); }

bool CBaseUserCmdPB::ComputeCRC() {
    std::string* prevChecksum = moveCRC;

    static auto SerializePartialToArray = signatures::SerializePartialToArray.GetPtrAs<bool (*)(void*, void*, int)>();
    static auto WriteMessage = signatures::WriteMessage.GetPtrAs<void (*)(void*, void*, size_t)>();
    static auto SetMessageData = signatures::SetMessageData.GetPtrAs<std::string* (*)(void*, void*, void*)>();

    CUtlBuffer buffer = CUtlBuffer();

    const int CRCSize = Checksum();
    buffer.EnsureCapacity(CRCSize + 1);
    buffer.m_Memory.nGrowSize = -1;
    if (!SerializePartialToArray(this, &buffer, CRCSize)) {
        CLogger::Log("Failed to serialize partial to array");
        return false;
    }

    uint64_t crc[4];
    memset(crc, 0, sizeof(crc));

    cachedBits |= 1;
    // probably does nothing
    uintptr_t* nHasBits = (uintptr_t*)(*(uintptr_t*)(uintptr_t(this) + 0x8) & 0xFFFFFFFFFFFFFFFC);
    if (*(uintptr_t*)(uintptr_t(this) + 0x8) & 1) nHasBits = (uintptr_t*)*nHasBits;
    // probably does nothing

    WriteMessage(crc, &buffer, CRCSize);
    moveCRC = SetMessageData(&moveCRC, crc, nHasBits);

    // if (crc[3] > 0x10 && crc[0]) {
    //     IMemAlloc::Get().Free((void*)crc[0]);
    // }

    CLogger::Log("checksum: {} -> {}\n", (uintptr_t)prevChecksum, (uintptr_t)moveCRC);
    return true;
}

void CUserCmd::SetBaseCmdButtons() { vt::CallMethod<void>(this, 15); }

void CUserCmd::GetBaseCmdButtons() { vt::CallMethod<void>(this, 16); }

CMsgQAngle* CMsgQAngle::Create(void* mem) { return signatures::CMsgQAngle.GetPtrAs<CMsgQAngle* (*)(void*)>()(mem); }

CMsgVector* CMsgVector::Create(void* mem) { return signatures::CMsgVector.GetPtrAs<CMsgVector* (*)(void*)>()(mem); }
