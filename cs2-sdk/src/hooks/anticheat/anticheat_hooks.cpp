#include "pch.hpp"

#include <hooks/anticheat/anticheat_hooks.hpp>
#include <logger/logger.hpp>

#include <signatures/signatures.hpp>
#include <constants/constants.hpp>

#include <hook/hook.hpp>

static CHook g_GetFunctions;
static void hkGetFunctions(uintptr_t* functions) { 
    SDK_LOG_PROLOGUE();
}

static CHook g_CSVCMsg_UserMessage_Setup;
static bool hkCSVCMsg_UserMessage_Setup(void* rcx, void* msg) {
    SDK_LOG_PROLOGUE();
	//return g_CSVCMsg_UserMessage_Setup.CallOriginal<bool>(rcx, msg);
    return false;
}

void CAntiCheatHooks::Initialize() {
    SDK_LOG_PROLOGUE();

    g_GetFunctions.Hook(signatures::GetFunctions.GetPtrAs<void*>(), SDK_HOOK(hkGetFunctions));
    g_CSVCMsg_UserMessage_Setup.Hook(signatures::CSVCMsg_UserMessage_Setup.GetPtrAs<void*>(), SDK_HOOK(hkCSVCMsg_UserMessage_Setup));
}
