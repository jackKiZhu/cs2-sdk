#include <pch.hpp>

#include <interfaces/localize.hpp>

#include <memory/memory.hpp>
#include <constants/constants.hpp>

#include <virtual/virtual.hpp>

CLocalize* CLocalize::Get() { 
    static const auto inst = CMemory::GetInterface(CConstants::LOCALIZE_LIB, "Localize_001");
    return inst.Get<CLocalize*>();
}

const char* CLocalize::FindSafe(const char* tokenName) { return vt::CallMethod<const char*>(this, 17, tokenName); }
