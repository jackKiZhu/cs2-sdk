#pragma once

struct KVID_t;

class CKeyValues
{
   public:
    uint64_t key;
    void* value;
    PAD(0x8);

    bool Load(const char* buffer, const KVID_t* kvid, const char* unk);
};

struct KVID_t
{
    const char* name;
    uint64_t pad[2];
};
