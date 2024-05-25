#include "pch.hpp"

#include <module/module.hpp>

#include <pointer/pointer.hpp>

#include <hash/fnv1a.hpp>

CModule::CModule(const char* name) {
    m_Name = name;
    m_Handle = nullptr;

    m_Begin = m_Size = 0;
}

CModule::~CModule() {
    if (!m_Handle) {
        return;
    }

#ifndef _WIN32
    dlclose(m_Handle);
#endif
}

bool CModule::Retrieve() {
    if (m_Handle) {
        return true;
    }

    InitializeHandle();
    InitializeBounds();

    return m_Handle != nullptr;
}

uintptr_t CModule::GetInterface(uint32_t versionHash) {
    if (!m_Handle) {
        return 0;
    }

    CPointer pCreateInterface = GetProcAddress("CreateInterface");
    if (!pCreateInterface.IsValid()) {
        return 0;
    }

    if (_interfaces.empty()) {
        // Used internally to register classes.
        struct InterfaceReg {
            std::add_pointer_t<uintptr_t()> m_CreateFn;
            const char* m_pName;
            InterfaceReg* m_pNext; // For the global list.
        };

        const InterfaceReg* s_pInterfaceRegs =
#ifdef _WIN32
            pCreateInterface.Absolute(3, 0).Dereference(1).Get<InterfaceReg*>();
#elif __linux__
                pCreateInterface.Absolute(1, 0).Absolute(19, 0).Dereference(1).Get<InterfaceReg*>();
#endif

        // save extra search
        std::uintptr_t rv = 0;

        for (; s_pInterfaceRegs; s_pInterfaceRegs = s_pInterfaceRegs->m_pNext) {
            auto hashedName         = FNV1A::Hash(s_pInterfaceRegs->m_pName);
            const auto address      = s_pInterfaceRegs->m_CreateFn();
            _interfaces[hashedName] = address;

            if (hashedName == versionHash) {
                rv = address;
            }
        }

        if (rv) {
            return rv;
        }
    }

    const auto it = _interfaces.find(versionHash);
    if (it != _interfaces.end()) {
        return it->second;
    }

    // or just throw exception here?
    return 0;
}

uintptr_t CModule::GetProcAddress(const char* procName) {
    uintptr_t rv = 0;
    if (m_Handle) {
#ifdef _WIN32
        rv = reinterpret_cast<uintptr_t>(::GetProcAddress(static_cast<HMODULE>(m_Handle), procName));
#else
        rv = reinterpret_cast<uintptr_t>(dlsym(m_Handle, procName));
#endif
    }

    return rv;
}

uintptr_t CModule::FindPattern(const std::span<const int>& pattern) const {
    
    if (!m_Handle) {
        return 0;
    }

    // Faster than pattern[] in debug builds because of _STL_VERIFY.
    const int* patternData   = pattern.data();
    const size_t patternSize = pattern.size();

    auto* start = reinterpret_cast<uint8_t*>(m_Begin);
    auto* end   = reinterpret_cast<uint8_t*>(m_Begin + m_Size - patternSize);

    for (std::uint8_t* current = start; current <= end; ++current) {
        current = std::find(current, end, patternData[0]);

        if (current == end) {
            break;
        }

        if (std::equal(pattern.begin() + 1, pattern.end(), current + 1,
                       [](auto opt, auto byte) { return opt == -1 || opt == byte; })) {
            return {uintptr_t(current)};
        }
    }

    return 0;
}

void CModule::InitializeHandle() {
#ifdef _WIN32
    m_Handle = GetModuleHandle(GetName());
#else
    m_Handle = dlopen(GetName(), RTLD_LAZY | RTLD_NOLOAD);
#endif
}

void CModule::InitializeBounds() {
    if (!m_Handle) {
        return;
    }

#ifdef _WIN32
    MODULEINFO mi;
    BOOL status = GetModuleInformation(GetCurrentProcess(), static_cast<HMODULE>(m_Handle), &mi, sizeof(mi));
    if (status == 0) {
        return;
    }

    SetBounds(reinterpret_cast<uintptr_t>(m_Handle), mi.SizeOfImage);
#elif __linux__
    dl_iterate_phdr(
        [](struct dl_phdr_info* info, size_t, void* data) {
            CModule* currentModule = static_cast<CModule*>(data);
            if (strstr(info->dlpi_name, currentModule->GetName()) != 0) {
                for (Elf64_Half i = 0; i < info->dlpi_phnum; ++i) {
                    const auto& phdr = info->dlpi_phdr[i];
                    if (phdr.p_type != PT_LOAD || !(phdr.p_flags & PF_X)) continue;

                    currentModule->SetBounds(info->dlpi_addr + phdr.p_vaddr, phdr.p_memsz);
                    return 1;
                }
            }

            return 0;
        },
        static_cast<void*>(this));
#endif
}

void CModule::SetBounds(uintptr_t begin, uintptr_t size) {
    m_Begin = begin;
    m_Size = size;
}
