#pragma once

#include <module/module.hpp>
#include <pointer/pointer.hpp>

using ModulePtr_t = std::shared_ptr<CModule>;

class CMemory {
   public:
    static CMemory& Get() {
        static CMemory inst;
        return inst;
    }

    static auto& GetModule(const char* libName) { return Get().GetModuleInternal(libName); }
    static auto GetInterface(const char* libName, const char* version) { return Get().GetInterfaceInternal(libName, version); }
    static auto GetProcAddress(const char* libName, const char* procName) { return Get().GetProcAddressInternal(libName, procName); }
    static auto SetSelf(HMODULE mod) { 
      Get().m_SelfModule.first = uintptr_t(mod);
      PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)mod;
      PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((uintptr_t)mod + dosHeader->e_lfanew);
      Get().m_SelfModule.second = ntHeader->OptionalHeader.SizeOfImage;
    }
    static auto GetSelf() { return Get().m_SelfModule; }

    void Initialize();

   private:
    ModulePtr_t& GetModuleInternal(const char* libName);

    CPointer GetInterfaceInternal(const char* libName, const char* version);
    CPointer GetProcAddressInternal(const char* libName, const char* procName);

    std::unordered_map<uint32_t, ModulePtr_t> m_CachedModules;
    std::pair<uintptr_t, size_t> m_SelfModule;
};
