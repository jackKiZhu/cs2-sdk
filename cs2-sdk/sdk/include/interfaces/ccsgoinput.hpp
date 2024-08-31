#pragma once

#include <math/types/vector.hpp>
#include <types/utlvector.hpp>
#include <memory/memory.hpp>

// this will have both CClientInput and CCSGOInput for simplicity's sake

#define MAX_SPLITSCREEN_PLAYERS 150

enum Buttons {
    IN_ATTACK = (1 << 0),
    IN_JUMP = (1 << 1),
    IN_DUCK = (1 << 2),
    IN_FORWARD = (1 << 3),
    IN_BACK = (1 << 4),
    IN_USE = (1 << 5),
    IN_CANCEL = (1 << 6),
    IN_LEFT = (1 << 7),
    IN_RIGHT = (1 << 8),
    IN_MOVELEFT = (1 << 9),
    IN_MOVERIGHT = (1 << 10),
    IN_SECOND_ATTACK = (1 << 11),
    IN_RUN = (1 << 12),
    IN_RELOAD = (1 << 13),
    IN_LEFT_ALT = (1 << 14),
    IN_RIGHT_ALT = (1 << 15),
    IN_SCORE = (1 << 16),
    IN_SPEED = (1 << 17),
    IN_WALK = (1 << 18),
    IN_ZOOM = (1 << 19),
    IN_FIRST_WEAPON = (1 << 20),
    IN_SECOND_WEAPON = (1 << 21),
    IN_BULLRUSH = (1 << 22),
    IN_FIRST_GRENADE = (1 << 23),
    IN_SECOND_GRENADE = (1 << 24),
    IN_MIDDLE_ATTACK = (1 << 25),
    IN_USE_OR_RELOAD = (1 << 26)
};

struct InterpInfo_t {
    int srcTick;
    int dstTick;
    float fraction;
};

struct Tickfrac_t {
    int tick;
    float fraction;
};

template <typename T>
struct RepeatedPtrField_t {
    struct Rep_t {
        int allocatedSize;
        T* elements[(std::numeric_limits<int>::max() - 2 * sizeof(int)) / sizeof(void*)];
    };

    void* arena;
    int currentSize;
    int totalSize;
    Rep_t* rep;
};

// reading is the same as in here: "F3 41 0F 10 47 ? 83 C9"
struct CSubtickInput {
    float when;       // 0x0 [0.f - 1.f]
    PAD(0x4);         // 0x4
    uint64_t button;  // 0x8
    union {
        bool held;           // 0x10
        float forwardDelta;  // 0x10
    };
    float sidemoveDelta;  // 0x14
};                        // Size: 0x18

#pragma pack(push, 1)
struct CMoveData {
    uint64_t buttonsHeld;            // 0x0
    uint64_t buttonsPressed;         // 0x8
    uint64_t buttonsReleased;        // 0x10
    uint64_t prevButtonsHeld;        // 0x18
    Vector moves;                    // 0x20
    int32_t mouseDx;                 // 0x2C
    int32_t mouseDy;                 // 0x30
    uint32_t subtickCount;           // 0x34
    CSubtickInput subtickMoves[12];  // 0x38
    Vector viewAngles;               // 0x158
    float fraction;                  // 0x164

    bool Push(uint64_t button, bool held, float when = 0.90f) {
        if (subtickCount >= 12) return false;
        subtickMoves[subtickCount++] = CSubtickInput{.when = when, .button = button, .held = held};
        return true;
    }

    uint8_t Pop(uint64_t button) {
        uint8_t c = 0;
        for (int i = 0; i < subtickCount; i++) {
            if (subtickMoves[i].button == button) {
                memmove(subtickMoves + i, subtickMoves + i + 1, (subtickCount - i - 1) * sizeof(CSubtickInput));
                subtickCount--;
                i--;
                c++;
            }
        }
        return c;
    }
};  // Size: 0x168
#pragma pack(pop)

class CBasePB {
   public:
    void* vftable;        // 0x0
    uint32_t hasBits;     // 0x8
    uint64_t cachedBits;  // 0xC

    size_t Checksum();
};
static_assert(sizeof(CBasePB) == 0x18);

class CMsgQAngle : public CBasePB {
   public:
    static CMsgQAngle* Create(void* mem = nullptr);

    Vector viewAngles;  // 0x18 (0xC)
};                      // Size: 0x24

class CMsgVector : public CBasePB {
   public:
    static CMsgVector* Create(void* mem = nullptr);

    Vector xyz;
    float w;
};

class CCSGOInterpolationInfoPB : public CBasePB {
   public:
    float fraction;
    int srcTick;
    int dstTick;
};

class CCSGOInputHistoryEntryPB : public CBasePB {
   public:
    CMsgQAngle* pViewCmd;                     // 0x18
    CMsgVector* pShootOriginCmd;              // 0x20
    CMsgVector* pTargetHeadOriginCmd;         // 0x28
    CMsgVector* pTargetAbsOriginCmd;          // 0x30
    CMsgQAngle* pTargetViewCmd;               // 0x38
    CCSGOInterpolationInfoPB* cl_interp;      // 0x40
    CCSGOInterpolationInfoPB* sv_interp0;     // 0x48
    CCSGOInterpolationInfoPB* sv_interp1;     // 0x50
    CCSGOInterpolationInfoPB* player_interp;  // 0x58
    int nRenderTickCount;                     // 0x60
    float flRenderTickFraction;               // 0x64
    int nPlayerTickCount;                     // 0x68
    float flPlayerTickFraction;               // 0x6C
    int nFrameNumber;                         // 0x70
    int nTargetEntIndex;                      // 0x74
};

class CInButtonState {
   public:
    PAD(0x8);
    uint64_t held;
    uint64_t changed;
    uint64_t scroll;
};
static_assert(sizeof(CInButtonState) == 0x20);

class CInButtonStatePB : public CBasePB {
   public:
    uint64_t held;
    uint64_t changed;
    uint64_t scroll;
};
static_assert(sizeof(CInButtonStatePB) == 0x30);

class CSubtickMoveStep : public CBasePB {
   public:
    uint64_t button;  // 0x18
    bool pressed;     // 0x20
    PAD(0x3);         // 0x21
    float when;       // 0x24
    float forward;    // 0x28
    float sidemove;   // 0x2C
};
static_assert(sizeof(CSubtickMoveStep) == 0x30);

class CBaseUserCmdPB : public CBasePB {
   public:
    RepeatedPtrField_t<CSubtickMoveStep> subticksMoveSteps;  // 0x18 (0x18)
    std::string* moveCRC;                                    // 0x30 (0x8)
    CInButtonStatePB* buttons;                               // 0x38 (0x8)
    CMsgQAngle* angles;                                      // 0x40 (0x8)
    int commandNumber;                                       // 0x48 (0x4)
    uint32_t tickcount;                                      // 0x4C (0x4)
    Vector moves;                                            // 0x50 (0xC)
    int32_t impulse;                                         // 0x5C (0x4)
    int32_t weaponSelect;                                    // 0x60 (0x4)
    int32_t randomSeed;                                      // 0x64 (0x4)
    int32_t mouseDx;                                         // 0x68 (0x4)
    int32_t mouseDy;                                         // 0x6C (0x4)
    uint32_t consumedServerAngleChanges;                     // 0x70 (0x4)
    int32_t cmdFlags;                                        // 0x74 (0x4)
    uint32_t pawnEntity;                                     // 0x78 (0x4)
    PAD(0x4);                                                // 0x7C (0x4)

    bool ComputeCRC();
};  // Size: 0x80

class CCSGOUserCmdPB {
   public:
    std::uint32_t hasBits;                                           // 0x0 (0x4)
    std::uint64_t cachedBits;                                        // 0x4 (0x8)
    RepeatedPtrField_t<CCSGOInputHistoryEntryPB> inputHistoryField;  // 0xC (0x18)
    CBaseUserCmdPB* baseCmd;                                         // 0x24 (0x8)
    bool wantsLeftHand;                                              // 0x2C (0x4)
    PAD(0x3);
    std::int32_t attack1HistoryIndex;  // 0x30 (0x4)
    std::int32_t attack2HistoryIndex;  // 0x34 (0x4)
    std::int32_t attack3HistoryIndex;  // 0x38 (0x4)

    size_t Checksum();
    char* Checksum(char* a2, char* a3);
};
static_assert(sizeof(CCSGOUserCmdPB) == 0x40);

class CUserCmd {
   public:
    void* vftable;               // 0x0 (0x8)
    PAD(0x10);    
    CCSGOUserCmdPB csgoUserCmd;  
    CInButtonState buttons;      
    PAD(0x20);                   

    CCSGOInputHistoryEntryPB* GetInputHistoryEntry(int i) {
        if (i >= csgoUserCmd.inputHistoryField.rep->allocatedSize || i >= csgoUserCmd.inputHistoryField.currentSize) return nullptr;

        return csgoUserCmd.inputHistoryField.rep->elements[i];
    }

    CSubtickMoveStep* GetSubtickMoveEntry(int i) {
        CBaseUserCmdPB* baseCmd = csgoUserCmd.baseCmd;
        if (!baseCmd) return nullptr;
        auto& steps = baseCmd->subticksMoveSteps;

        if (i >= steps.rep->allocatedSize || i >= steps.currentSize) return nullptr;

        return steps.rep->elements[i];
    }

    void SetSubTickAngle(const Vector& angView) {
        for (int i = 0; i < this->csgoUserCmd.inputHistoryField.rep->allocatedSize; i++) {
            CCSGOInputHistoryEntryPB* entry = GetInputHistoryEntry(i);
            if (!entry || !entry->pViewCmd) continue;
            entry->pViewCmd->viewAngles = angView;
        }
    }

    void SetBaseCmdButtons();
    void GetBaseCmdButtons();
};  // Size: 0x88
// static_assert(sizeof(CUserCmd) == 0x98);

class CInputFrame {
   public:
    Tickfrac_t render;  // 0x0
    Tickfrac_t player;  // 0x8
    Vector viewAngles;  // 0x10
    Vector shootPos;    // 0x1C
    int targetIndex;    // 0x28
    Vector headPos;          // 0x2C
    Vector absOrigin;        // 0x38
    Vector eyeAngles;        // 0x44
    uint32_t frameNumber;    // 0x50
    int historyEntryIndex;   // 0x54
    PAD(0x8);                // 0x58
};                           // Size: 0x60

enum EInButtonState : uint64_t {
    IN_BUTTON_IDLE = 0,
    IN_BUTTON_HELD = 2,
    IN_BUTTON_PRESSED = 6,
    IN_BUTTON_RELEASED = 8,
    IN_BUTTON_HELD_SCROLL = 10,
    IN_BUTTON_SCROLL = 12,
};

class CCSGOInput {
   public:
    static CCSGOInput* Get();

    PAD(0x250);                                  // 0x0 (0x250)
    CUserCmd commands[MAX_SPLITSCREEN_PLAYERS];  // 0x250 (0x4FB0)
    PAD(0x99);     
    bool inThirdPerson;  
    PAD(0x6);
    Vector thirdPersonAngles;
    PAD(0xE);
    int sequenceNumber;                                                            
    double platFloatTime;                       
    CMoveData moveData;                          // 0x52C0 (0x168)
    uint32_t lastSwitchWeaponTick;               // 0x5428 (0x4)
    PAD(0x1BC);                                  
    CUtlVector<CMoveData> moves;                 
    PAD(0x20);                                   
    int inputHandlerCount;                       
    PAD(0x4);                                    
    void* inputHandlers;                         // 0x54F0 (0x8)
    PAD(0x18);                                   // 0x54F8 (0x10)
    int attackHistoryIndex1;                     // 0x5508 (0x4)
    int attackHistoryIndex2;                     // 0x550C (0x4)
    int attackHistoryIndex3;                     // 0x5510 (0x4)
    PAD(0x4);                                    // 0x5514 (0x4)
    CUtlVector<CInputFrame> frameHistory;        // 0x5518 (0x18)
    int lastProcessedMoveDataIndex;              // 0x5530 (0x4)

    // 0x5508

    CUserCmd* GetUserCmd();
    CUserCmd* GetUserCmd(uint32_t sequenceNumber);
    CMoveData* GetMoveData(int index);

    void Scroll(uint64_t button);
    void Release(uint64_t button);

    static EInButtonState GetButtonState(uint64_t buttons[3], uint64_t button) {
        return (EInButtonState)((button & buttons[0]) * 0x1 + (button & buttons[1]) * 0x2 + (button & buttons[2]) * 0x4);
    }

    static bool IsButtonPressed(uint64_t buttons[3], uint64_t button) {
        bool multiKeys = (button & (button - 1));
        if (!multiKeys) {
            EInButtonState state =
                (EInButtonState)((button & buttons[0]) * 0x1 + (button & buttons[1]) * 0x2 + (button & buttons[2]) * 0x4);
            if (state > IN_BUTTON_HELD) return true;
            return buttons[0] & button;
        }

        uint64_t i = button;
        uint64_t key = 0;
        if (button) {
            while (true) {
                if (i & 1) {
                    uint64_t mask = 1ull << key;
                    EInButtonState state =
                        (EInButtonState)((mask & buttons[0]) * 0x1 + (mask & buttons[1]) * 0x2 + (mask & buttons[2]) * 0x4);
                    if (state > IN_BUTTON_HELD) return true;
                }
                key++;
                i >>= 1;
                if (!i) return buttons[0] & button;
            }
        }

        return false;
    }
};
