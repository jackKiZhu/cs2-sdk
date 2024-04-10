#include <pch.hpp>

#include <bindings/baseviewmodel.hpp>
#include <signatures/signatures.hpp>
#include <memory/memory.hpp>

CViewmodelMaterialInfo* C_CSGOViewModel::GetMaterialInfo() { 
 static auto FindViewmodelMaterial =
        signatures::FindViewmodelMaterial.GetPtr().Absolute(1, 0).Get<uint32_t (*)(void*, void*, uint32_t, bool*)>();
    if (!FindViewmodelMaterial) return nullptr;

    static uint32_t offset = signatures::FindViewmodelMaterial.GetPtr().GetField<uint32_t>(15);  // 0xF80
    CViewmodelMaterialInfo* materialInfo = CPointer(this).GetFieldPtr(offset).Get<CViewmodelMaterialInfo*>();
    return materialInfo;
}

void C_CSGOViewModel::InvalidateViewmodelMaterial() {
    CViewmodelMaterialInfo* materialInfo = GetMaterialInfo();
    if (!materialInfo) return;

    CViewmodelMaterialRecord* record = materialInfo->GetRecord(VIEWMODEL_MATERIAL_GLOVES);
    if (record) record->typeIndex = -1;
}

CViewmodelMaterialRecord* CViewmodelMaterialInfo::GetRecord(ViewModelMaterial identifier) {
    for (uint32_t i = 0; i < count; ++i) {
        CViewmodelMaterialRecord& record = records[i];
        if (record.identifier == identifier) 
            return &record;
    }
    return nullptr;
}
