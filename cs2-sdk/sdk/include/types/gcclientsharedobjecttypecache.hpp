#pragma once

#include <memory/memory.hpp>

#include <types/utlvector.hpp>

class CSharedObject;

class CGCClientSharedObjectTypeCache {
   public:
    bool AddObject(CSharedObject* object);
    CSharedObject* RemoveObject(CSharedObject* soIndex);

    template <typename T>
    CUtlVector<T>& GetVecObjects() {
        return CPointer(this).GetField<CUtlVector<T>>(0x8);
    }
};
