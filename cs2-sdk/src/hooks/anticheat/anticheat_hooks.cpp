#include "pch.hpp"

#include <intrin.h>

#include <hooks/anticheat/anticheat_hooks.hpp>
#include <logger/logger.hpp>

#include <signatures/signatures.hpp>
#include <constants/constants.hpp>

#include <hook/hook.hpp>

struct CDllVerification {
    void *vftable;
    void *CDllPtr;                         // 0x0008
    char pad_0010[8];                      // 0x0010
    void *GetTotalFilesLoaded;             // 0x0018
    void *CountFilesNeedTrustCheck;        // 0x0020
    void *CountFilesCompletedTrustCheck;   // 0x0028
    void *BSecureAllowed;                  // 0x0030
    void *CountItemsToReport;              // 0x0038
    uint64_t N0000015C;                    // 0x0040
    uint64_t N0000015D;                    // 0x0048
    uint64_t N0000015F;                    // 0x0050
    uint64_t N00000160;                    // 0x0058
    uint64_t KERNEL32;                     // 0x0060
    uint64_t KERNELBASE;                   // 0x0068
    uint64_t ntdll;                        // 0x0070
    uint64_t gameoverlayrenderer;          // 0x0078
    uint64_t NtQueryInformationThread;     // 0x0080
    uint64_t LoadLibraryExW;               // 0x0088
    uint64_t GetCurrentThreadStackLimits;  // 0x0090
    uint64_t GetVersionExA;                // 0x0098
    uint64_t KernelBaseHook;               // 0x00A0
    uint64_t GetVersionExADeref;           // 0x00A8
    uint64_t B0;                           // 0x00B0
    uint64_t KERNEL32_2;                   // 0x00B8
    uint64_t KERNEL32Hook;                 // 0x00C0
    uint64_t N00000170;                    // 0x00C8
    uint64_t N00000171;                    // 0x00D0
    char pad_00D8[1848];                   // 0x00D8
};                                         // Size: 0x0810

static CHook g_GetFunctions;
static void hkGetFunctions(CDllVerification *dll) {
    dll->GetTotalFilesLoaded = nullptr;
    dll->CountFilesNeedTrustCheck = nullptr;
    dll->CountFilesCompletedTrustCheck = nullptr;
    dll->BSecureAllowed = nullptr;
    dll->CountItemsToReport = nullptr;
    dll->NtQueryInformationThread = 0;
    dll->LoadLibraryExW = 0;
    dll->GetCurrentThreadStackLimits = 0;
    dll->GetVersionExA = 0;
    dll->KernelBaseHook = 0;
    dll->GetVersionExADeref = 0;
    dll->KERNEL32Hook = 0;
}

static CHook g_GetFunctions2;
static void hkGetFunctions2(CDllVerification *dll) {
    dll->GetTotalFilesLoaded = nullptr;
    dll->CountFilesNeedTrustCheck = nullptr;
    dll->CountFilesCompletedTrustCheck = nullptr;
    dll->BSecureAllowed = nullptr;
    dll->CountItemsToReport = nullptr;
    dll->NtQueryInformationThread = 0;
    dll->LoadLibraryExW = 0;
    dll->GetCurrentThreadStackLimits = 0;
    dll->GetVersionExA = 0;
    dll->KernelBaseHook = 0;
    dll->GetVersionExADeref = 0;
    dll->KERNEL32Hook = 0;
}

static CHook g_CSVCMsg_UserMessage_Setup;
static bool hkCSVCMsg_UserMessage_Setup(void *CSVCMsg_UserMessage, int type, void *CUserMessage) {
    CLogger::Log("CSVCCMsg_UserMessage of type {} has been blasted!", type);

    if (type == 368) return g_CSVCMsg_UserMessage_Setup.CallOriginal<bool>(CSVCMsg_UserMessage, type, CUserMessage);

    return false;
}

void CAntiCheatHooks::Initialize() {
    SDK_LOG_PROLOGUE();

    g_GetFunctions.Hook(signatures::GetFunctions.GetPtrAs<void *>(), SDK_HOOK(hkGetFunctions));
    g_GetFunctions2.Hook(signatures::GetFunctions2.GetPtrAs<void *>(), SDK_HOOK(hkGetFunctions2));
    g_CSVCMsg_UserMessage_Setup.Hook(signatures::CSVCMsg_UserMessage_Setup.GetPtrAs<void *>(), SDK_HOOK(hkCSVCMsg_UserMessage_Setup));
}

// 48 89 4C 24 ? 48 81 EC ? ? ? ? 48 C7 44 24 - DllVerification
// 48 89 4C 24 ? 48 83 EC ? 48 83 3D - DllVerification2
// 44 88 4C 24 ? 4C 89 44 24 ? 48 89 54 24 ? 48 89 4C 24 ? B8 - CUserMessage_DllStatus
