#include <pch.hpp>

#include <interfaces/memalloc.hpp>
#include <signatures/signatures.hpp>
#include <memory/memory.hpp>
#include <constants/constants.hpp>

//CMemAlloc* CMemAlloc::Get() { 
//  //return signatures::MemAlloc.GetPtrAs<CMemAlloc*>();
//    return CMemory::Get().GetProcAddress(CConstants::TIER_LIB, "g_pMemAlloc").Get<CMemAlloc*>();
//}

void* CMemAlloc::Alloc(size_t size) {
    static auto fn = CMemory::GetProcAddress(CConstants::TIER_LIB, "MemAlloc_AllocFunc").Get<void* (*)(size_t)>();
    if (!fn) return nullptr;
    return fn(size);
}

void CMemAlloc::Free(void* ptr) {
    static auto fn = CMemory::GetProcAddress(CConstants::TIER_LIB, "MemAlloc_FreeFunc").Get<void (*)(void*)>();
    if (!fn) return;
    fn(ptr);
}
