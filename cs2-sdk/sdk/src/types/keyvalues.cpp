#include <pch.hpp>

#include <types/keyvalues.hpp>
#include <types/utlbuffer.hpp>
#include <types/utlmemory.hpp>
#include <memory/memory.hpp>
#include <constants/constants.hpp>
#include <signatures/signatures.hpp>

void CKeyValues3::FreeAllocation() {
    static const auto fn = signatures::Keyvalues3FreeAllocation.GetPtrAs<CKeyValues3* (*)(CKeyValues3*, uint32_t)>();
    fn(this, 0);
}

bool CKeyValues3::LoadFromString(const char* textKV, const char* path) {
    KVID_t kv3ID;
    kv3ID.name = "generic";
    kv3ID.guid_low = 0x469806E97412167C;
    kv3ID.guid_high = 0xE73790B53EE6F2AF;

    CUtlBuffer buffer(textKV, strlen(textKV) + 1, CUtlBuffer::READ_ONLY);
    buffer.m_Memory.nGrowSize = -1; // hack to prevent CUtlMemory from freeing stack allocated array (TODO: find a better way)

    SetType(1, 6);

    return LoadKV3FromKV3OrKV1(nullptr, &buffer, kv3ID, path);
}

bool CKeyValues3::LoadKV3FromKV3OrKV1(void* utlStrError, CUtlBuffer* bufTextKV, const KVID_t& kv3ID, const char* path) {
    static const auto fn = CMemory::GetProcAddress(
        CConstants::TIER_LIB, "?LoadKV3FromKV3OrKV1@@YA_NPEAVKeyValues3@@PEAVCUtlString@@PEAVCUtlBuffer@@AEBUKV3ID_t@@PEBD@Z");
    return fn.Call<bool (*)(CKeyValues3*, void*, CUtlBuffer*, const KVID_t&, const char*)>(this, utlStrError, bufTextKV, kv3ID, path);
}

void CKeyValues3::SetType(uint8_t type1, uint8_t type2) {
    static const auto fn = signatures::Keyvalues3SetType.GetPtrAs<CKeyValues3* (*)(CKeyValues3*, uint8_t, uint8_t)>();
    fn(this, type1, type2);
}
