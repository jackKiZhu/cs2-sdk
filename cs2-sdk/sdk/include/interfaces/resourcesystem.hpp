#pragma once

#include <virtual/virtual.hpp>
#include <memory/memory.hpp>
#include <constants/constants.hpp>

#include <interfaces/materialsystem.hpp>

struct ResourceBinding_t;

struct ResourceArray_t {
    uint64_t count;
    CMaterial2** resources;
    uint64_t pad[3];
};

class CResourceSystem {
   public:
    static CResourceSystem* Get() {
        static const auto instance = CMemory::GetInterface(CConstants::RESOURCESYSTEM_LIB, "ResourceSystem013");
        return instance.Get<CResourceSystem*>();
    }

    void* QueryInterface(const char* interfaceName);

    VIRTUAL_METHOD(void, EnumerateResources, 38, (uint64_t typeHash, ResourceArray_t* resources, uint8_t flag),
                   (this, typeHash, resources, flag));
};

class CResourceHandleUtils {
   public:
    void DeleteResource(const ResourceBinding_t* binding);
};
