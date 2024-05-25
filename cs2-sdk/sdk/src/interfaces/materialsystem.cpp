#include <pch.hpp>

#include <interfaces/materialsystem.hpp>

#include <memory/memory.hpp>
#include <constants/constants.hpp>
#include <signatures/signatures.hpp>
#include <virtual/virtual.hpp>

CMaterialSystem* CMaterialSystem::Get() {
    static const auto inst = CMemory::GetInterface(CConstants::MATERIALSYSTEM_LIB, "VMaterialSystem2_001");
    return inst.Get<CMaterialSystem*>();
}

uint64_t MaterialKeyVar_t::FindKey(const char* name) {
    static auto oFindKeyVar = signatures::FindKeyVar.GetPtrAs<uint64_t (*)(const char*, uint32_t, int)>();
    return oFindKeyVar ? oFindKeyVar(name, 0x12, 0x31415926) : 0x0;
}

void CSceneData::SetShaderType(const char* shaderName) {
    static auto oSetMaterialShaderType =
        signatures::SetMaterialShaderType.GetPtrAs<uint64_t (*)(void*, MaterialKeyVar_t, const char*, uint8_t)>();
    if (!oSetMaterialShaderType) return;
    MaterialKeyVar_t key(0x162C1777, "shader");
    oSetMaterialShaderType(this, key, shaderName, 0x13u);
}

void CSceneData::SetMaterialFunction(const char* functionName, int value) {
    static auto oSetMaterialFunction = signatures::SetMaterialFunction.GetPtrAs<uint64_t (*)(void*, MaterialKeyVar_t, int, uint8_t)>();
    if (!oSetMaterialFunction) return;
    MaterialKeyVar_t key(functionName, true);
    oSetMaterialFunction(this, key, value, 0x13u);
}

CMaterial2*** CMaterialSystem::FindOrCreateFromResource(CMaterial2*** out, const char* materialName) {
    return vt::CallMethod<CMaterial2***>(this, 14, out, materialName);
}

CMaterial2** CMaterialSystem::CreateMaterial(void* out, const char* materialName, void* data, unsigned int unk, uint8_t unk2) {
    return vt::CallMethod<CMaterial2**>(this, 29, out, materialName, data, unk, unk2);
}

void CMaterialSystem::SetCreateDataByMaterial(CMaterial2*** const in, const void* data) {
    return vt::CallMethod<void>(this, 38, in, data);
}
