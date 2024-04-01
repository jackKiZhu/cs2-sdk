#include "pch.hpp"
#include <interfaces/globalvars.hpp>

#include <signatures/signatures.hpp>

CGlobalVars* CGlobalVars::Get() { 
    static auto inst = signatures::GetGlobalVars.GetPtrAs<CGlobalVars**>();
    return *inst;
}
