#include <pch.hpp>

#include <types/utlbuffer.hpp>
#include <memory/memory.hpp>
#include <constants/constants.hpp>

#include <interfaces/memalloc.hpp>

CUtlBuffer::CUtlBuffer(const void* buffer, int a3, uint8_t a4) {
    static const auto fn = CMemory::GetProcAddress(CConstants::TIER_LIB, "??0CUtlBuffer@@QEAA@PEBXHH@Z");
    fn.Call<CUtlBuffer* (*)(CUtlBuffer*, const void*, int, uint8_t)>(this, buffer, a3, a4);
}


// ??0CUtlBuffer@@QEAA@HHH@Z
CUtlBuffer::CUtlBuffer(int a2, int a3, uint8_t a4) {
    static const auto fn = CMemory::GetProcAddress(CConstants::TIER_LIB, "??0CUtlBuffer@@QEAA@HHH@Z");
    fn.Call<CUtlBuffer* (*)(CUtlBuffer*, int, int, uint8_t)>(this, a2, a3, a4);
}

CUtlBuffer::~CUtlBuffer() { /*IMemAlloc::Get().Free(this);*/ }

void CUtlBuffer::EnsureCapacity(int a2) {
    static const auto fn = CMemory::GetProcAddress(CConstants::TIER_LIB, "?EnsureCapacity@CUtlBuffer@@QEAAXH@Z");
    fn.Call<void (*)(CUtlBuffer*, int)>(this, a2);
}
