#include <pch.hpp>

#include <bindings/baseflex.hpp>
#include <signatures/signatures.hpp>

void C_BaseModelEntity::SetModel(const char* model) {
    static auto SetModel = signatures::SetModel.GetPtrAs<void (*)(C_BaseModelEntity*, const char*)>();
    if (SetModel) SetModel(this, model);
}
