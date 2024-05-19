#pragma once

class CCSPlayerController;

struct KeyString_t {
    KeyString_t(const char* key) : u64(-1), key(key) {}
    uint64_t u64;
    const char* key;
};

class CGameEvent {
   public:
    const char* GetName();
    bool GetBool(const std::string_view token);
    // client.dll & 89 85 08 0B 00 00 ("damage_taken")
    int GetIntFromHash(const std::string_view token);

    float GetFloat(const std::string_view token);
    // client.dll & 48 89 54 24 10 48 89 4C 24 08 55 53 56 57 41 55 41 56 41 57 48 8D AC 24 D0 DF FF FF ("weapon")
    const char* GetString(const std::string_view token);

    // client.dll & 4C 8B A8 80 00 00 00 ("killer")
    CCSPlayerController* GetPlayerController(const std::string_view token);

    int64_t GetInt(const std::string_view eventName);

    //   @ida: server.dll -> ["48 8D 4D A7 48 8B B8 ? ? ? ? 33" + 0x7] / sizeof(std::uintptr_t)   @XREF: "weapon"
    void SetString(const std::string_view token, const char* value);
};
