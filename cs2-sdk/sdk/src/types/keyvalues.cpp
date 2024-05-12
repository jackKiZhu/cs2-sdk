#include <pch.hpp>

#include <types/keyvalues.hpp>
#include <memory/memory.hpp>
#include <constants/constants.hpp>

bool CKeyValues::Load(const char* buffer, const KVID_t* kvid, const char* unk) { 
    static const auto fn = CMemory::GetProcAddress(CConstants::TIER_LIB, "?LoadKV3@@YA_NPEAVKeyValues3@@PEAVCUtlString@@PEBDAEBUKV3ID_t@@2@Z");
    return fn.Call<bool (*)(CKeyValues*, const char*, const KVID_t*, const char*)>(this, buffer, kvid, unk);
}
