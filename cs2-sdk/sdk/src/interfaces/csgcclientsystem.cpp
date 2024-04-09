#include <pch.hpp>

#include <interfaces/csgcclientsystem.hpp>
#include <signatures/signatures.hpp>
#include <memory/memory.hpp>

CGCClientSharedObjectCache* CSGCClient::FindSOCache(SOID_t* id, bool createIfMissing) { 
	static auto FindSOCache = signatures::FindSOCache.GetPtrAs<CGCClientSharedObjectCache*(__thiscall*)(void*, SOID_t*, bool)>();
    return FindSOCache ? FindSOCache(this, id, createIfMissing) : nullptr;
}

CSGCClientSystem* CSGCClientSystem::Get() { 
	static CSGCClientSystem* sgcClientSystem = signatures::GetCCSGCClientSystem.GetPtrAs<CSGCClientSystem*>();
    return sgcClientSystem;
}

CSGCClient* CSGCClientSystem::GetSGCClient() { return CPointer(this).GetFieldPtr(0xB8).Get<CSGCClient*>(); }
