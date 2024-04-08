#include <pch.hpp>

#include <types/gcclientsharedobjecttypecache.hpp>
#include <virtual/virtual.hpp>

bool CGCClientSharedObjectTypeCache::AddObject(CSharedObject* object) { return vt::CallMethod<bool>(this, 1, object); }

CSharedObject* CGCClientSharedObjectTypeCache::RemoveObject(CSharedObject* soIndex) { return vt::CallMethod<CSharedObject*>(this, 3, soIndex); }
