#pragma once

#include <hash/murmurhash2.hpp>

#define STRINGTOKEN_MURMURHASH_SEED 0x31415926

#pragma pack(push, 8)
class CUtlStringToken {
   public:
    CUtlStringToken() = default;

    explicit CUtlStringToken(std::string_view keyName) {
        uHashCode = MurmurHash2(keyName.data(), keyName.size(), STRINGTOKEN_MURMURHASH_SEED);
        szDebugName = keyName.data();
    }

    constexpr CUtlStringToken(const uint32_t uHashCode, const char* szKeyName) : uHashCode(uHashCode), szDebugName(szKeyName) {}
    bool operator==(const CUtlStringToken& other) const { return (other.uHashCode == uHashCode); }
    bool operator!=(const CUtlStringToken& other) const { return (other.uHashCode != uHashCode); }
    bool operator<(const CUtlStringToken& other) const { return (uHashCode < other.uHashCode); }

   public:
    uint32_t uHashCode = 0U; // 0x00
    const char* szDebugName = nullptr; // 0x08 
};
#pragma pack(pop)

inline consteval CUtlStringToken MakeStringToken(std::string_view keyName) {
    return {MurmurHash2(keyName.data(), keyName.size(), STRINGTOKEN_MURMURHASH_SEED), keyName.data()};
}
