#pragma once

#include <virtual/virtual.hpp>
#include <types/utlvector.hpp>
#include <types/utlmap.hpp>

class CEconItemDefinition;

inline constexpr uint64_t Helper_GetAlternateIconKeyForWeaponPaintWearItem(uint16_t defIndex, uint32_t paintID, uint32_t wear) {
    return (defIndex << 16) + (paintID << 2) + wear;
}

struct AlternateIconData_t {
    const char* sSimpleName;
    const char* sLargeSimpleName;

   private:
    char pad0[0x10];  // no idea
};

#pragma pack(push, 1)
class CPaintKit {
   public:
    uint64_t id;               // 0x0
    char* sName;               // 0x8
    char* sDescriptionString;  // 0x10
    char* sDescriptionTag;     // 0x18
   private:
    char pad_20[0x8];

   public:
    char* sPattern;       // 0x28
    char* sNormal;        // 0x30
    char* sLogoMaterial;  // 0x38
   private:
    char pad_40[0x4];

   public:
    uint32_t nRarity;           // 0x44
    uint32_t nStyle;            // 0x48
    uint32_t rgbaColor[4];      // 0x4c
    uint32_t rgbaLogoColor[4];  // 0x5c
   private:
    char pad_6c[0x4];

   public:
    float flWearRemapMin;         // 0x70
    float flWearRemapMax;         // 0x74
    uint8_t nFixedSeed;           // 0x78
    uint8_t uchPhongExponent;     // 0x79
    uint8_t uchPhongAlbedoBoost;  // 0x7a
    uint8_t uchPhongIntensity;    // 0x7b
    float flPatternScale;         // 0x7c
    float flPatternOffsetXStart;  // 0x80
    float flPatternOffsetXEnd;    // 0x84
    float flPatternOffsetYStart;  // 0x88
    float flPatternOffsetYEnd;    // 0x8c
    float flPatternRotateStart;   // 0x90
    float flPatternRotateEnd;     // 0x94
    float flLogoScale;            // 0x98
    float flLogoOffsetX;          // 0x9c
    float flLogoOffsetY;          // 0xa0
    float flLogoRotation;         // 0xa4
    bool bIgnoreWeaponSizeScale;  // 0xa8
   private:
    char pad_a9[0x3];

   public:
    uint32_t nViewModelExponentOverrideSize;  // 0xac
    bool bOnlyFirstMaterial;                  // 0xb0
    bool bUseNormalModel;                     // 0xb1
    bool bUseLegacyModel;                     // 0xb2
   private:
    char pad_b3[0x1];

   public:
    float pearlescent;  // 0xb4
    char* sVmtPath;     // 0xb8
   private:
    char pad_c0[0x8];

   public:
    char* sCompositeMaterialPath;  // 0xc8
    void* kvVmtOverrides;          // 0xd0
   private:
    char pad_d8[0x8];

   public:
};  // Size: 0xe0
#pragma pack(pop)

class CEconItemSchema {
   public:
    void* GetAttributeDefinitionInterface(int attribIndex);
    CUtlMap<int, CEconItemDefinition*>& GetSortedItemDefinitionMap();
    CUtlMap<uint64_t, AlternateIconData_t>& GetAlternateIconsMap();
    CUtlMap<int, CPaintKit*>& GetPaintKits();
};
