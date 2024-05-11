#include <pch.hpp>

#include <interfaces/memalloc.hpp>
#include <signatures/signatures.hpp>

CMemAlloc* CMemAlloc::Get() { 
  return signatures::MemAlloc.GetPtrAs<CMemAlloc*>();
}
