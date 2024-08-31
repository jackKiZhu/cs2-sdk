#include <pch.hpp>

#include <input/input.hpp>
#include <logger/logger.hpp>

void CInputHandler::Poll(CCSGOInput* input) {
    buttonsScroll = 0;
    prevButtonsHeld = buttonsHeld;

    return;

    [[unlikely]] if (input->moves.m_Size == 0)
        return;

    for (int i = 0; i < input->moves.m_Size; ++i) {
        move = input->moves.AtPtr(i);
        [[unlikely]] if (!move)
            continue;

        uint64_t pressed = 0;

        uint64_t lastButton = 0;
        for (uint32_t j = 0; j < move->subtickCount; ++j) {
            CSubtickInput& subInput = move->subtickMoves[j];
            if (subInput.when >= 1.f) break;

            if (subInput.held) {
                lastButton = subInput.button;
                pressed |= subInput.button;
            } else {
                pressed &= ~subInput.button;
                if (lastButton == subInput.button) buttonsScroll |= subInput.button;

                // erase the subinput
                if (move->subtickCount - j > 0)
                    memmove(&subInput, &move->subtickMoves[j + 1], sizeof(CSubtickInput) * (move->subtickCount - j));
                move->subtickCount--;
            }
        }

        buttonsHeld |= pressed;

        for (uint32_t j = 0; j < move->subtickCount; ++j) {
            CSubtickInput& subInput = move->subtickMoves[j];
            if (subInput.when >= 1.f) break;
            if (move->subtickCount > 11) break;

            if (subInput.held && pressed & subInput.button) {
                CSubtickInput& releaseInput = move->subtickMoves[move->subtickCount++];
                releaseInput.button = subInput.button;
                releaseInput.held = false;
                releaseInput.when = subInput.when;
                move->buttonsHeld &= ~subInput.button;
                move->buttonsPressed |= subInput.button;
                move->buttonsReleased |= subInput.button;
            } 
        }
    }
}

void CInputHandler::Press(uint64_t button, float when) {
    if (!move) return;

    // we always need to input 2 subtick inputs
    [[unlikely]] if (move->subtickCount > 10) {
        CLogger::Log("Too many subticks already queued, input ignored");
        return;
    }

    if (move->subtickCount > 0) when = move->subtickMoves[move->subtickCount - 1].when;

    CSubtickInput& pressInput = move->subtickMoves[move->subtickCount++];
    CSubtickInput& releaseInput = move->subtickMoves[move->subtickCount++];

    pressInput.button = releaseInput.button = button;

    if (false && IsHeld(button)) {
        // interupt hold
        pressInput.held = false;
        pressInput.when = when + 0.05f;
        // press it back down
        releaseInput.held = true;
        releaseInput.when = when + 0.1f;
    } else {
        // press it
        pressInput.held = true;
        pressInput.when = when;
        // release it
        releaseInput.held = false;
        releaseInput.when = when;
    }
}

bool CInputHandler::Release(uint64_t button) {
    if (!move) return false;

    bool removed = false;
    for (uint32_t i = 0; i < move->subtickCount; ++i) {
        CSubtickInput& subInput = move->subtickMoves[i];
        if (subInput.button != button) continue;
        if (subInput.held) {
            if (move->subtickCount - i > 0)
                memmove(&subInput, &move->subtickMoves[i + 1], sizeof(CSubtickInput) * (move->subtickCount - i));
            move->subtickCount--;
            i--;
            removed = true;
        }
    }

    /* move->subtickCount - i > 0 */

    if (!IsReleased(button)) {
        if (move->subtickCount >= 12) {
            CLogger::Log("Too many subticks already queued, input ignored");
            return false;
        }

        CSubtickInput& subInput = move->subtickMoves[move->subtickCount++];
        subInput.button = button;
        subInput.held = false;
        subInput.when = 1.f;
    }

    return removed;
}

void CInputHandler::Dump() {
    if (!move) return;
    if (move->subtickCount < 1) return;
    CLogger::Log("================================================");
    CLogger::Log("Subtick count: {}", move->subtickCount);
    for (uint32_t i = 0; i < move->subtickCount; ++i) {
        CSubtickInput& subInput = move->subtickMoves[i];
        CLogger::Log("Subtick {}: button 0x{:X}, held {}, when {:.2f}", i, subInput.button, subInput.held, subInput.when);
    }
}
