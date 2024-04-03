#pragma once

// this will have both CClientInput and CCSGOInput for simplicity's sake

#define MAX_SPLITSCREEN_PLAYERS 150

enum Buttons {
    IN_ATTACK = 1 << 0,
    IN_JUMP = 1 << 1,
    IN_DUCK = 1 << 2,
    IN_FORWARD = 1 << 3,
    IN_BACK = 1 << 4,
    IN_USE = 1 << 5,
    IN_MOVELEFT = 1 << 9,
    IN_MOVERIGHT = 1 << 10,
    IN_ATTACK2 = 1 << 11,
    IN_SCORE = 1 << 16,
    IN_BULLRUSH = 1 << 22
};

struct ButtonState_t {
    PAD(0x8);
    uint64_t value;
    uint64_t valueChanged;
    uint64_t valueScroll;
};
static_assert(sizeof(ButtonState_t) == 0x20);

// reading is the same as in here: "F3 41 0F 10 47 ? 83 C9"
struct CSubtickMove {
    float when; // 0x0 [0.f - 1.f]
    PAD(0x4); // 0x4
    uint64_t lastPressedButtons; // 0x8
    float analog_forward_delta; // 0x10 acts weirdly
    float analog_sidemove_delta; // 0x14 acts weirdly
}; // Size: 0x18

#pragma pack(push, 1)
struct CMoveData {
    uint64_t buttonsHeld; // 0x0
    uint64_t buttonsChanged; // 0x8
    uint64_t buttonsScroll; // 0x10
    uint64_t prevButtonsHeld; // 0x18
    Vector moves; // 0x20
    int32_t mouseDx; // 0x2C
    int32_t mouseDy; // 0x30
    uint32_t subtickCount; // 0x34 
    CSubtickMove subtickMoves[12]; // 0x38
    Vector viewAngles;               // 0x158
    float time;                     // 0x164
}; // Size: 0x158
#pragma pack(pop)

class CBasePB {
   public:
    PAD(0x18);  // 0x0 (0x18)
};

class CMsgQAngle : public CBasePB {
   public:
    Vector viewAngles; // 0x18 (0xC)
}; // Size: 0x24

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

class CBaseUserCmdPB {
   public:
    PAD(0x40);  // 0x0 (0x40)
    CMsgQAngle* msgAngle;   // 0x40 (0x8)
    int commandNumber; // 0x48 (0x4)
    uint32_t tickcount; // 0x4C (0x4)
    Vector moves; // 0x50 (0xC)
    // PAD(0x24); // 0x5C (0x24)
    int32_t impulse;        // 0x5C (0x4)
    int32_t weaponSelect;  // 0x60 (0x4)
    int32_t randomSeed;     // 0x64 (0x4)
    int32_t mouseDx; // 0x68 (0x4)
    int32_t mouseDy; // 0x6C (0x4)
    uint32_t consumedServerAngleChanges;  // 0x70 (0x4)
    int32_t cmdFlags; // 0x74 (0x4)
    uint32_t pawnEntity; // 0x78 (0x4)
    PAD(0x4); // 0x7C (0x4)
}; // Size: 0x80

class CCSGOUserCmdPB {
   public:
    uint32_t tickcount; // 0x0 (0x4)
    PAD(0x4); // 0x4 (0x4)
    void* inputHistory; // 0x8 (0x8)

    CCSGOInputHistoryEntryPB* GetInputHistoryEntry(int tick) {
        if (tick < tickcount) {
            CCSGOInputHistoryEntryPB** arrTickList = std::bit_cast<CCSGOInputHistoryEntryPB**>(std::bit_cast<uintptr_t>(inputHistory) + 0x8);
            return arrTickList[tick];
        }
        return nullptr;
    }
}; // Size: 0x10

class CUserCmd {
   public:
    void* vftable; // 0x0 (0x8)
    uint64_t qword8; // 0x8 (0x8)
    uint32_t flags; // 0x10 (0x4)
    PAD(0xC); // 0x14 (0xC)
    CCSGOUserCmdPB csgoUserCmd; // 0x20 (0x10)
    CBaseUserCmdPB* baseCmd; // 0x30 (0x8)

    // 0x70 = realtime
    /// 0x50  = button,s

 #if 0
    PAD(0x18); // 0x38 (0x18)
    uint64_t qword50; // 0x50 (0x8)
    PAD(32); // 0x58 (0x20)
    std::byte b; // 0x78 (0x1)
    PAD(7); // 0x79 (0x7)
    int32_t dword80; // 0x80 (0x4)
    PAD(0x4); // 0x84 (0x4)
#endif
    PAD(0x10); // 0x38 (0x10)
    ButtonState_t buttons; // 0x48 (0x20)
    PAD(0x20); // 0x68 (0x20)

    void SetSubTickAngle(const Vector& angle) {
        for (int i = 0; i < csgoUserCmd.tickcount; i++) {
            CCSGOInputHistoryEntryPB* entry = csgoUserCmd.GetInputHistoryEntry(i);
            if (!entry || !entry->pViewCmd) continue;
            entry->pViewCmd->viewAngles = angle;
        }
    }
}; // Size: 0x88 
static_assert(sizeof(CUserCmd) == 0x88);

class CCSGOInput {
   public:
    static CCSGOInput* Get();

    PAD(0x250); // 0x0 (0x250)
    CUserCmd commands[MAX_SPLITSCREEN_PLAYERS]; // 0x250 (0x4FB0)
    PAD(0x1); // 0x5200 (0x1)
    bool inThirdperson; // 0x5201 (0x1)
    PAD(0x6);
    Vector tihrdPersonAngle;  // 0x5208
    PAD(0x10);
    int sequenceNumber; // 0x5224 (0x4)
    int sequenceNumber2;   // 0x5228 (0x4)
    PAD(0x4); // 0x522C (0x4)
    double platFloatTime; // 0x5230 (0x8)
    #if 0
    uint64_t buttonsHeld; // 0x5238 (0x8)
    uint64_t buttonsChanged; // 0x5240 (0x8)
    uint64_t buttonsScroll; // 0x5248 (0x8)
    uint64_t buttonsHeld2; // 0x5250 (0x8)
    Vector moves; // 0x5258 (0xC)
    int32_t mouseDx; // 0x5264 (0x4)s
    int32_t mouseDy;  // 0x5268 (0x4)
    uint32_t scrollAmount; // 0x526C (0x4)
    PAD(0x120); // 0x526C (0x124)
    #else
    CMoveData moveData; // 0x5238 (0x158)
    #endif

    CUserCmd* GetUserCmd();
    CUserCmd* GetUserCmd(uint32_t sequenceNumber);
};

void ValidateUserCmd(const Vector& original, CUserCmd* cmd, CCSGOInputHistoryEntryPB* entry);

