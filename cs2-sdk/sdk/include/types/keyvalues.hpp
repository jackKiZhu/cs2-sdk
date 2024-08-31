#pragma once

class CUtlBuffer;

struct KVID_t {
    const char* name;
    uint64_t guid_low;
    uint64_t guid_high;
};

class CKeyValues3 {
   public:
    CKeyValues3() : unk(0), memory(0) {}

    CKeyValues3(const char* szTextKV, const char* szTextPath = nullptr) : unk(0), memory(0) { LoadFromString(szTextKV, szTextPath); }

    ~CKeyValues3() {
        if (memory) FreeAllocation();
    }

    void FreeAllocation();
    bool LoadFromString(const char* textKV, const char* path = nullptr);
    bool LoadKV3FromKV3OrKV1(void* utlStrError, CUtlBuffer* bufTextKV, const KVID_t& kv3ID, const char* path);
    void SetType(uint8_t type1, uint8_t type2);

   private:
    uint64_t unk;
    void* memory;
    PAD(0x8);
};

