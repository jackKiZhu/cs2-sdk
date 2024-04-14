#include <pch.hpp>

#include <interfaces/resourcesystem.hpp>
#include <virtual/virtual.hpp>

void* CResourceSystem::QueryInterface(const char* interfaceName) { return vt::CallMethod<void*>(this, 2, interfaceName); }

void CResourceHandleUtils::DeleteResource(const ResourceBinding_t* binding) { return vt::CallMethod<void>(this, 2, binding); }
