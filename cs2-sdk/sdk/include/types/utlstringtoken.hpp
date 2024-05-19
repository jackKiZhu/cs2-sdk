#pragma once

#include <fnv/fnv1a.hpp>

#define STRINGTOKEN_MURMURHASH_SEED 0x31415926

#pragma pack(push, 8)
class CUtlStringToken {
   public:
    explicit CUtlStringToken(const char* szKeyName) {
        uHashCode = FNV1A::Hash(szKeyName);
        szDebugName = szKeyName;
    }

    constexpr CUtlStringToken(const FNV1A uHashCode, const char* szKeyName) : uHashCode(uHashCode), szDebugName(szKeyName) {}

    bool operator==(const CUtlStringToken& other) const { return (other.uHashCode == uHashCode); }

    bool operator!=(const CUtlStringToken& other) const { return (other.uHashCode != uHashCode); }

    bool operator<(const CUtlStringToken& other) const { return (uHashCode < other.uHashCode); }

   public:
    uint32_t uHashCode = 0U; // 0x00
    const char* szDebugName = nullptr; // 0x08 
};
#pragma pack(pop)
