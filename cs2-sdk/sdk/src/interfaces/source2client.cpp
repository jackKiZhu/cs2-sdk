#include "pch.hpp"

#include <interfaces/source2client.hpp>

#include <memory/memory.hpp>
#include <constants/constants.hpp>
#include <virtual/virtual.hpp>

CSource2Client* CSource2Client::Get() {
    static const auto inst = CMemory::GetInterface(CConstants::CLIENT_LIB, "Source2Client002");
    return inst.Get<CSource2Client*>();
}

CEconItemSystem* CSource2Client::GetEconItemSystem() { 
  // 74 ? 48 8B C8 E8 ? ? ? ? 48 8B F8 48 8D 05 ? ? ? ? 48 89 7B ? 48 89 03 EB ? 48 8B DF 48 8B 7C 24 ? 48 8B C3
    return vt::CallMethod<CEconItemSystem*>(this, 123);
}
