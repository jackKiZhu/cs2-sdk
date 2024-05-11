#pragma once

class CMemAlloc
{
   public:
  static CMemAlloc* Get();

  virtual void* Alloc(size_t size) = 0;
  virtual void* Realloc(void* ptr, size_t size) = 0;
  virtual void Free(void* ptr) = 0;
};
