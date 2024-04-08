#pragma once

#include <types/utlvector.hpp>

#pragma pack(push, 1)
class CEconItemDefinition {
   public:
    bool IsWeapon();
    bool IsKnife(bool excludeDefault);
    bool IsGlove(bool excludeDefault);

    void* vtable;          // 0x0
    void* m_pKVItem;       // 0x8
    uint16_t m_nDefIndex;  // 0x10
   private:
    char pad_12[0x1e];

   public:
    bool m_bEnabled;  // 0x30
   private:
    char pad_31[0xf];

   public:
    uint8_t m_unMinItemLevel;           // 0x40
    uint8_t m_unMaxItemLevel;           // 0x41
    uint8_t m_nItemRarity;              // 0x42
    uint8_t m_nItemQuality;             // 0x43
    uint8_t m_nForcedItemQuality;       // 0x44
    uint8_t m_nDefaultDropItemQuality;  // 0x45
    uint8_t m_nDefaultDropQuantity;     // 0x46
   private:
    char pad_47[0x19];

   public:
    uint8_t m_nPopularitySeed;  // 0x60
   private:
    char pad_61[0x7];

   public:
    void* m_pPortraitsKV;     // 0x68
    char* m_pszItemBaseName;  // 0x70
    bool m_bProperName;       // 0x78
   private:
    char pad_79[0x7];

   public:
    char* m_pszItemTypeName;  // 0x80
   private:
    char pad_88[0x8];

   public:
    char* m_pszItemDesc;              // 0x90
    uint32_t m_iExpirationTimeStamp;  // 0x98
    uint32_t m_iCreationTimeStamp;    // 0x9c
    char* m_pszInventoryModel;        // 0xa0
    char* m_pszInventoryImage;        // 0xa8
   private:
    char pad_b0[0x18];

   public:
    int m_iInventoryImagePosition[2];  // 0xc8
    int m_iInventoryImageSize[2];      // 0xd0
    char* m_pszBaseDisplayModel;       // 0xd8
    bool m_bLoadOnDemand;              // 0xe0
   private:
    char pad_e1[0x1];

   public:
    bool m_bHideBodyGroupsDeployedOnly;  // 0xe2
   private:
    char pad_e3[0x5];

   public:
    char* m_pszWorldDisplayModel;        // 0xe8
    char* m_pszHolsteredModel;           // 0xf0
    char* m_pszWorldExtraWearableModel;  // 0xf8
    uint32_t m_iStickerSlots;            // 0x100
   private:
    char pad_104[0x4];

   public:
    char* m_pszIconDefaultImage;  // 0x108
    bool m_bAttachToHands;        // 0x110
    bool m_bAttachToHandsVMOnly;  // 0x111
    bool m_bFlipViewModel;        // 0x112
    bool m_bActAsWearable;        // 0x113
   private:
    char pad_114[0x24];

   public:
    uint32_t m_iItemType;  // 0x138
   private:
    char pad_13c[0x4];

   public:
    char* m_pszBrassModelOverride;  // 0x140
    char* m_pszZoomInSoundPath;     // 0x148
    char* m_pszZoomOutSoundPath;    // 0x150
   private:
    char pad_158[0x18];

   public:
    uint32_t m_nSoundMaterialID;    // 0x170
    bool m_bDisableStyleSelection;  // 0x174
   private:
    char pad_175[0x13];

   public:
    char* m_pszParticleFile;          // 0x188
    char* m_pszParticleSnapshotFile;  // 0x190
   private:
    char pad_198[0x40];

   public:
    char* m_pszItemClassname;        // 0x1d8
    char* m_pszItemLogClassname;     // 0x1e0
    char* m_pszItemIconClassname;    // 0x1e8
    char* m_pszDefinitionName;       // 0x1f0
    bool m_bHidden;                  // 0x1f8
    bool m_bShouldShowInArmory;      // 0x1f9
    bool m_bBaseItem;                // 0x1fa
    bool m_bFlexibleLoadoutDefault;  // 0x1fb
    bool m_bImported;                // 0x1fc
    bool m_bOnePerAccountCDKEY;      // 0x1fd
   private:
    char pad_1fe[0xa];

   public:
    char* m_pszArmoryDesc;  // 0x208
   private:
    char pad_210[0x8];

   public:
    char* m_pszArmoryRemap;  // 0x218
    char* m_pszStoreRemap;   // 0x220
    char* m_pszClassToken;   // 0x228
    char* m_pszSlotToken;    // 0x230
    uint32_t m_iDropType;    // 0x238
   private:
    char pad_23c[0x4];

   public:
    char* m_pszHolidayRestriction;  // 0x240
    uint32_t m_iSubType;            // 0x248
   private:
    char pad_24c[0xc];

   public:
    uint32_t m_unEquipRegionMask;          // 0x258
    uint32_t m_unEquipRegionConflictMask;  // 0x25c
   private:
    char pad_260[0x50];

   public:
    bool m_bPublicItem;              // 0x2b0
    bool m_bIgnoreInCollectionView;  // 0x2b1
   private:
    char pad_2b2[0x36];

   public:
    int m_iLoadoutSlot;  // 0x2e8
   private:
    char pad_2ec[0x94];
};
#pragma pack(pop)
