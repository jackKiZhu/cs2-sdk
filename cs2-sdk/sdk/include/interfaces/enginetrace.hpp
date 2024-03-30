#pragma once

#include <math/types/vector.hpp>

struct Ray_t {
    Vector start;
    Vector end;
    Vector mins;
    Vector maxs;
    PAD(0x4);
    uint8_t type;
};

struct SurfaceData_t {
    PAD(0x8);
    float penetrationModifier;
    float damageModifier;
    PAD(0x4);
    int material;
};

struct TraceHitboxData_t {
    PAD(0x58);
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
    PAD(0x10); // 0x18
    uint32_t contents; // 0x28
    PAD(0x4C); // 0x2C
    Vector start; // 0x78
    Vector end; // 0x84
    PAD(0x1C); // 0x90
    float fraction; // 0xAC
    int dwordb0; // 0xB0
    uint16_t wordb4; // 0xB4
    char b6; // 0xB6
    uint16_t wordb7; // 0xB7 
};

struct TraceFilter_t {
    uint64_t mask;
    PAD(0x8 * 2);
    uint32_t skipHandles[4];
    uint16_t collisions[2];
    PAD(0x5);

    virtual ~TraceFilter_t(){};
    virtual bool Unk() { return true; };
};

class C_BaseEntity;

class CEngineTrace {
   public:
    static CEngineTrace* Get();

    bool TraceShape(Ray_t* ray, const Vector& start, const Vector& end, TraceFilter_t* filter, GameTrace_t* trace);
    bool TraceShape(const Vector& start, const Vector& end, C_BaseEntity* skip, uint64_t mask, char a6, GameTrace_t* trace);
};


