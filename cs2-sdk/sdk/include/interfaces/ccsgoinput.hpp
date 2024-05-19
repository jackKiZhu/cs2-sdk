#pragma once

#include <math/types/vector.hpp>
#include <types/utlvector.hpp>

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
    float when;                       // 0x0 [0.f - 1.f]
    PAD(0x4);                         // 0x4
    uint64_t button;                  // 0x8
    union {
        bool held;            // 0x10
        float forwardDelta;   // 0x10 
    };
    float sidemoveDelta;      // 0x14
};                                    // Size: 0x18

#pragma pack(push, 1)
struct CMoveData {
    uint64_t buttonsHeld;            // 0x0
    uint64_t buttonsChanged;         // 0x8
    uint64_t buttonsScroll;          // 0x10
    uint64_t prevButtonsHeld;        // 0x18
    Vector moves;                    // 0x20
    int32_t mouseDx;                 // 0x2C
    int32_t mouseDy;                 // 0x30
    uint32_t subtickCount;           // 0x34
    CSubtickInput subtickMoves[12];  // 0x38
    Vector viewAngles;               // 0x158
    float fraction;                  // 0x164

    void Scroll(uint64_t button) {
        if (subtickCount <= 10) {
            buttonsHeld |= button;
            buttonsScroll |= button;
            subtickCount += 2;
            subtickMoves[subtickCount - 2] = subtickMoves[subtickCount - 1] = CSubtickInput{
                .when = 0.95f,
                .button = button,
                .held = true
            };
            subtickMoves[subtickCount - 1].held = 0;
        }
    }
};  // Size: 0x168
#pragma pack(pop)

class CBasePB {
    void* vftable;
    uint32_t hasBits;
    uint64_t cachedBits;
};
static_assert(sizeof(CBasePB) == 0x18);

class CMsgQAngle : public CBasePB {
   public:
    Vector viewAngles;  // 0x18 (0xC)
};                      // Size: 0x24

class CMsgVector : public CBasePB {
   public:
    Vector xyz;
    float w;
};

class CCSGOInterpolationInfo : public CBasePB {
   public:
    float fraction;
    int srcTick;
    int dstTick;
};

class CCSGOInputHistoryEntryPB : public CBasePB {
   public:
    CMsgQAngle* pViewCmd;                   // 0x18
    CMsgVector* pShootOriginCmd;            // 0x20
    CMsgVector* pTargetHeadOriginCmd;       // 0x28
    CMsgVector* pTargetAbsOriginCmd;        // 0x30
    CMsgQAngle* pTargetViewCmd;             // 0x38
    CCSGOInterpolationInfo* cl_interp;      // 0x40
    CCSGOInterpolationInfo* sv_interp0;     // 0x48
    CCSGOInterpolationInfo* sv_interp1;     // 0x50
    CCSGOInterpolationInfo* player_interp;  // 0x58
    int nRenderTickCount;                   // 0x60
    float flRenderTickFraction;             // 0x64
    int nPlayerTickCount;                   // 0x68
    float flPlayerTickFraction;             // 0x6C
    int nFrameNumber;                       // 0x70
    int nTargetEntIndex;                    // 0x74
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

struct CSubtickMoveStepContainer
{
    int a;
    PAD(0x4);
    CSubtickMoveStep* moves[12];
};

class CBaseUserCmdPB : public CBasePB {
   public:
    RepeatedPtrField_t<CSubtickMoveStep> subticksMoveSteps; // 0x18 (0x18)      
    const char* moveCRC;                                    // 0x30 (0x8)
    CInButtonStatePB* buttons;                              // 0x38 (0x8)
    CMsgQAngle* angles;                                     // 0x40 (0x8)
    int commandNumber;                    // 0x48 (0x4)
    uint32_t tickcount;                   // 0x4C (0x4)
    Vector moves;                         // 0x50 (0xC)
    int32_t impulse;                      // 0x5C (0x4)
    int32_t weaponSelect;                 // 0x60 (0x4)
    int32_t randomSeed;                   // 0x64 (0x4)
    int32_t mouseDx;                      // 0x68 (0x4)
    int32_t mouseDy;                      // 0x6C (0x4)
    uint32_t consumedServerAngleChanges;  // 0x70 (0x4)
    int32_t cmdFlags;                     // 0x74 (0x4)
    uint32_t pawnEntity;                  // 0x78 (0x4)
    PAD(0x4);                             // 0x7C (0x4)
};                                        // Size: 0x80

class CCSGOUserCmdPB {
   public:
    std::uint32_t hasBits;
    std::uint64_t cachedBits;
    RepeatedPtrField_t<CCSGOInputHistoryEntryPB> inputHistoryField;
    CBaseUserCmdPB* baseCmd;
    bool wantsLeftHand;
    std::int32_t nAttack3StartHistoryIndex;
    std::int32_t nAttack1StartHistoryIndex;
    std::int32_t nAttack2StartHistoryIndex;
}; 
static_assert(sizeof(CCSGOUserCmdPB) == 0x40);

class CUserCmd {
   public:
    void* vftable;               // 0x0 (0x8)
    CCSGOUserCmdPB csgoUserCmd;  // 0x8
    CInButtonState buttons;      // 0x48
    PAD(0x20);                   // 0x68

    CCSGOInputHistoryEntryPB* GetInputHistoryEntry(int i) {
        if (i >= csgoUserCmd.inputHistoryField.rep->allocatedSize || i >= csgoUserCmd.inputHistoryField.currentSize)
            return nullptr;

        return csgoUserCmd.inputHistoryField.rep->elements[i];
    }

    CSubtickMoveStep* GetSubtickMoveEntry(int i) {
        CBaseUserCmdPB* baseCmd = csgoUserCmd.baseCmd;
        if (!baseCmd) return nullptr;
        auto& steps = baseCmd->subticksMoveSteps;

        if (i >= steps.rep->allocatedSize || i >= steps.currentSize)
            return nullptr;

        return steps.rep->elements[i];
    }

    void SetSubTickAngle(const Vector& angView) {
        for (int i = 0; i < this->csgoUserCmd.inputHistoryField.rep->allocatedSize; i++) {
            CCSGOInputHistoryEntryPB* entry = GetInputHistoryEntry(i);
            if (!entry || !entry->pViewCmd) continue;
            entry->pViewCmd->viewAngles = angView;
        }
    }
};  // Size: 0x88
static_assert(sizeof(CUserCmd) == 0x88);

class CCSGOInput {
   public:
    static CCSGOInput* Get();

    PAD(0x250);                                  // 0x0 (0x250)
    CUserCmd commands[MAX_SPLITSCREEN_PLAYERS];  // 0x250 (0x4FB0)
    PAD(0x1);                                    // 0x5200 (0x1)
    bool inThirdperson;                          // 0x5201 (0x1)
    PAD(0x6);
    Vector thirdPersonAngle;  // 0x5208
    PAD(0x10);
    int sequenceNumber;             // 0x5224 (0x4)
    int moveDataCount;              // 0x5228 (0x4)
    PAD(0x4);                       // 0x522C (0x4)
    double platFloatTime;           // 0x5230 (0x8)
    CMoveData moveData;             // 0x5238 (0x158)
    uint32_t lastSwitchWeaponTick;  // 0x53a0
    PAD(0x84);
    CUtlVector<CMoveData> moves;  // 0x5428

    CUserCmd* GetUserCmd();
    CUserCmd* GetUserCmd(uint32_t sequenceNumber);
};

