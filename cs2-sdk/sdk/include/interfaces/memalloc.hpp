#pragma once

class CMemAlloc
{
  public:
    static CMemAlloc& Get() {
       static CMemAlloc instance;
       return instance;
    }

    void* Alloc(size_t size);
    void Free(void* ptr);
  
};
