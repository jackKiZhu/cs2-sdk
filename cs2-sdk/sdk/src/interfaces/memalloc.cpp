#include <pch.hpp>

#include <interfaces/memalloc.hpp>

#include <virtual/virtual.hpp>

void* IMemAlloc::Alloc(size_t size) {
    return vt::CallMethod<void*>(this, 1, size);
}

void IMemAlloc::Free(void* ptr) {
    return vt::CallMethod<void>(this, 3, ptr);
}

void IMemAlloc::ReAlloc(void* ptr, size_t size) {
    return vt::CallMethod<void>(this, 2, ptr, size);
}
