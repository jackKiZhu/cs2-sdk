#pragma once

#include <math/types/vector.hpp>
#include <array>

class C_CSPlayerPawn;

struct Ray_t {
    Vector start; // 0x0
    Vector end;   // 0xC
    Vector mins;  // 0x18
    Vector maxs;  // 0x24
    PAD(0x4);     // 0x30
    uint8_t type; // 0x34
};

struct SurfaceData_t {
    PAD(0x8);
    float penetrationModifier;
    float damageModifier;
    PAD(0x4);
    int material;
};

struct TraceHitboxData_t {
    PAD(0x38);
    int hitgroup;
    PAD(0x4);
    int hitbox;
};

class C_BaseEntity;

struct GameTrace_t {
    GameTrace_t();

    void* surface; // 0x0
    C_BaseEntity* hitEntity; // 0x8
    TraceHitboxData_t* hitboxData; // 0x10
    PAD(0x38); // 0x18
    uint32_t contents;             // 0x50
    PAD(0x24);          // 0x54
    Vector start;                  // 0x78
    Vector end;         // 0x84
    Vector normal;                 // 0x90
    Vector position;    // 0x9C
    PAD(0x4);                      // 0xA8
    float fraction;     // 0xAC
    int dwordb0;                   // 0xB0
    uint16_t wordb4;    // 0xB4
    char allSolid;                 // 0xB6
    char startSolid;               // 0xB7
    PAD(0x4C);
};

struct TraceFilter_t {
    uint64_t vt;  // 0x0
    uint64_t mask;  // 0x8
    std::array<std::int64_t, 2> v1;
    std::array<std::int32_t, 4> skipHandles;
    std::array<std::int16_t, 2> collisions;
    std::int16_t m_v2;
    std::uint8_t m_v3;
    std::uint8_t m_v4;
    std::uint8_t m_v5;

    TraceFilter_t(std::uint64_t mask, C_CSPlayerPawn* skip1, C_CSPlayerPawn* skip2, int layer);

    virtual ~TraceFilter_t(){};
    virtual bool Unk() { return true; };
};

class C_BaseEntity;

class CEngineTrace {
   public:
    static CEngineTrace* Get();

    bool TraceShape(Ray_t* ray, const Vector& start, const Vector& end, TraceFilter_t* filter, GameTrace_t* trace);
    bool TraceShape(const Vector& start, const Vector& end, C_BaseEntity* skip, uint64_t mask, char a6, GameTrace_t* trace);
    //bool TraceShape(const Vector& start, const Vector& end, C_BaseEntity* skip, uint64_t mask, char a6, GameTrace_t* trace);
};


