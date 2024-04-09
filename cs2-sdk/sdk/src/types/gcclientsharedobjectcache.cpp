#include <pch.hpp>

#include <types/gcclientsharedobjectcache.hpp>
#include <signatures/signatures.hpp>

CGCClientSharedObjectTypeCache* CGCClientSharedObjectCache::CreateBaseTypeCache(int classID) { 
	static auto CreateBaseTypeCache = signatures::CreateBaseTypeCache.GetPtrAs<CGCClientSharedObjectTypeCache*(__thiscall*)(void*, int)>();
    return CreateBaseTypeCache ? CreateBaseTypeCache(this, classID) : nullptr;
}
