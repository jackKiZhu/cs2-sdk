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

// 48 89 4C 24 ? 48 81 EC ? ? ? ? 48 C7 44 24 - DllVerification
// 48 89 4C 24 ? 48 83 EC ? 48 83 3D - DllVerification2
// 44 88 4C 24 ? 4C 89 44 24 ? 48 89 54 24 ? 48 89 4C 24 ? B8 - CUserMessage_DllStatus
