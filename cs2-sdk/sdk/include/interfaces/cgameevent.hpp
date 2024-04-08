#pragma once

class CCSPlayerController;

struct GameEventStruct_t {
    GameEventStruct_t(const char* key) : u64(0), key(key) {}
    uint64_t u64;
    const char* key;
};

class CGameEvent {
   public:
    const char* GetName();
    CCSPlayerController* GetPlayerController(const char* key);
};
