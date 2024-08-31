#pragma once

#include <memory/memory.hpp>
#include <constants/constants.hpp>

class IMemAlloc
{
  public:
    static IMemAlloc* Get() {
       static const auto instance = CMemory::GetProcAddress(CConstants::TIER_LIB, "g_pMemAlloc");
        return *instance.Get<IMemAlloc**>();
    }

    void* Alloc(size_t size);
    void Free(void* ptr);
    void ReAlloc(void* ptr, size_t size);
  
};
