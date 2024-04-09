#pragma once


enum EEconItemQuality {
    IQ_UNDEFINED = -1,
    IQ_NORMAL,
    IQ_GENUINE,
    IQ_VINTAGE,
    IQ_UNUSUAL,
    IQ_UNIQUE,
    IQ_COMMUNITY,
    IQ_DEVELOPER,
    IQ_SELFMADE,
    IQ_CUSTOMIZED,
    IQ_STRANGE,
    IQ_COMPLETED,
    IQ_HAUNTED,
    IQ_TOURNAMENT,
    IQ_FAVORED
};

enum EEconItemRarity { IR_DEFAULT, IR_COMMON, IR_UNCOMMON, IR_RARE, IR_MYTHICAL, IR_LEGENDARY, IR_ANCIENT, IR_IMMORTAL };

// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/shared/econ/econ_item_constants.h#L39
enum EEconTypeID {
    k_EEconTypeItem = 1,
    k_EEconTypePersonaDataPublic = 2,
    k_EEconTypeGameAccountClient = 7,
    k_EEconTypeGameAccount = 8,
    k_EEconTypeEquipInstance = 31,
    k_EEconTypeDefaultEquippedDefinitionInstance = 42,
    k_EEconTypeDefaultEquippedDefinitionInstanceClient = 43,
    k_EEconTypeCoupon = 45,
    k_EEconTypeQuest = 46,
};

class CSharedObject;

class CEconItem {
    void SetDynamicAttributeValue(int index, void* value);
    void SetDynamicAttributeValueString(int index, const char* value);

   public:
    static CEconItem* CreateInstance();
    void Destruct(bool b = true);

    void SetPaintKit(float kit) { SetDynamicAttributeValue(6, &kit); }
    void SetPaintSeed(float seed) { SetDynamicAttributeValue(7, &seed); }
    void SetPaintWear(float wear) { SetDynamicAttributeValue(8, &wear); }
    void SetStatTrak(int count) { SetDynamicAttributeValue(80, &count); }
    void SetStatTrakType(int type) { SetDynamicAttributeValue(81, &type); }
    void SetCustomName(const char* pName) { SetDynamicAttributeValueString(111, pName); }

    char pad0[0x10];  // 2 vtables
    uint64_t m_ulID;
    uint64_t m_ulOriginalID;
    void* m_pCustomDataOptimizedObject;
    uint32_t m_unAccountID;
    uint32_t m_unInventory;
    uint16_t m_unDefIndex;
    uint16_t m_unOrigin : 5;
    uint16_t m_nQuality : 4;
    uint16_t m_unLevel : 2;
    uint16_t m_nRarity : 4;
    uint16_t m_dirtybitInUse : 1;
    int16_t m_iItemSet;
    int m_bSOUpdateFrame;
    uint8_t m_unFlags;
};
