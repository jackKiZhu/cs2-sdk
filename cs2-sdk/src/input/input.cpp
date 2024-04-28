#include <pch.hpp>

#include <input/input.hpp>
#include <logger/logger.hpp>

void CInputHandler::Poll(CCSGOInput* input) {
    [[unlikely]]
    if (input->subtickMoves.m_Size == 0) return;

    move = input->subtickMoves.AtPtr(input->subtickMoves.m_Size - 1);
    [[unlikely]]
    if (!move) return;

    buttonsScroll = 0;
    prevButtonsHeld = buttonsHeld;

    uint64_t lastButtonPressed = 0;
    for (uint32_t i = 0; i < move->subtickCount; ++i) {
        CSubtickInput& subInput = move->subtickMoves[i];
        if (subInput.held) {
            buttonsHeld |= subInput.button;
            lastButtonPressed = subInput.button;
        } else {         
            buttonsHeld &= ~subInput.button;
            if ( lastButtonPressed == subInput.button )
            {
                lastButtonPressed = 0;
                buttonsScroll |= subInput.button;
            }
        }
    }
}

void CInputHandler::Press(uint64_t button, float when) {
    if (!move) return;

    // we always need to input 2 subtick inputs
    [[unlikely]]
    if ( move->subtickCount > 10 )
    {
        CLogger::Log("Too many subticks already queued, input ignored");
        return;
    }

    if (move->subtickCount > 0) 
        when = move->subtickMoves[move->subtickCount - 1].when;
    
    CSubtickInput& pressInput = move->subtickMoves[move->subtickCount++];
    CSubtickInput& releaseInput = move->subtickMoves[move->subtickCount++];

    pressInput.button = releaseInput.button = button;

    if (IsHeld(button)) {
        // interupt hold
        pressInput.held = false;
        pressInput.when = when + 0.05f;
        // press it back down
        releaseInput.held = true;
        releaseInput.when = when + 0.1f;
    } 
    else {
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
