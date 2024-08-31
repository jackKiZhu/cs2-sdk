#include "pch.hpp"

#include <intrin.h>

#include <hooks/anticheat/anticheat_hooks.hpp>

#include <hooks/anticheat/symbols.hpp>

#include <logger/logger.hpp>

#include <signatures/signatures.hpp>
#include <constants/constants.hpp>
#include <memory/memory.hpp>

#include <hook/hook.hpp>

#define RIP_SANITY_CHECK(Rip, BaseAddress, ModuleSize) (Rip > BaseAddress) && (Rip < (BaseAddress + ModuleSize))

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

    enum {
        CUserMessageSayText2 = 118,
        CUserMessageLagCompensationError = 155,
        CUserMessageRequestDllStatus = 156,
        CUserMessageRequestUtilAction = 157,
        CUserMessageUtilMsgResponse = 158,    // this
        CUserMessageDllStatus = 159,          // this
        CUserMessageRequestInventory = 160,
        CUserMessageInventoryResponse = 161,  // this
        CUserMessageRequestDiagnostic = 162,
        CUserMessageDiagnosticResponse = 163,
        CUserMessageExtraUserData = 164,
        CUserMessageNotifyResponseFound = 165,
        CUserMessagePlayResponseConditional = 166,
        CCSUsrMsg_PlayerDecalDigitalSignature = 368,
    };

    if (type == CCSUsrMsg_PlayerDecalDigitalSignature)
        return g_CSVCMsg_UserMessage_Setup.CallOriginal<bool>(CSVCMsg_UserMessage, type, CUserMessage);

    return false;
}

static CHook g_DiagnosticMessageResponse;
static void hkDiagnosticMessageResponse(void *CUserMessage, uint32_t idk) { 
  CLogger::Log("DiagnosticMessageResponse has been blasted!"); 
}

static CHook g_SendNetMessage;
static void hkSendNetMessage(void *netChannel, void* a2, void* protobufBinding, void* message, bool reliable) {
    uintptr_t returnAddress = (uintptr_t)_ReturnAddress();

    CLogger::Log("Sent {} message from {:#x}!", *(int *)((uintptr_t)message + 0x20 + 0x8), returnAddress - CMemory::GetModule(CConstants::CLIENT_LIB)->GetBaseAddress());
    g_SendNetMessage.CallOriginal<void>(netChannel, a2, protobufBinding, message, reliable);
}

void CAntiCheatHooks::Initialize() {
    SDK_LOG_PROLOGUE();

    //g_GetFunctions.Hook(signatures::GetFunctions.GetPtrAs<void *>(), SDK_HOOK(hkGetFunctions));
    //g_GetFunctions2.Hook(signatures::GetFunctions2.GetPtrAs<void *>(), SDK_HOOK(hkGetFunctions2));
    g_CSVCMsg_UserMessage_Setup.Hook(signatures::CSVCMsg_UserMessage_Setup.GetPtrAs<void *>(), SDK_HOOK(hkCSVCMsg_UserMessage_Setup));
    //g_DiagnosticMessageResponse.Hook(signatures::DiagnosticMessageResponse.GetPtrAs<void *>(), SDK_HOOK(hkDiagnosticMessageResponse));
 
    void *netChannel = signatures::CNetChannel.GetPtrAs<void *>();
    g_SendNetMessage.VHook(netChannel, 48, SDK_HOOK(hkSendNetMessage));

    syms::initialize();

    #if 0
    if (!instrumentation::initialize()) {
        CLogger::Log("[+] Failed to initialize instrumentation");
    }

    curThread = GetCurrentThread();
    auto ntdll = CMemory::GetModule("ntdll.dll");
    instrumentation::hooked.push_back(ntdll->GetProcAddress("NtQueryInformationProcess"));
    instrumentation::hooked.push_back(ntdll->GetProcAddress("NtQueryInformationThread"));
    instrumentation::hooked.push_back(ntdll->GetProcAddress("NtProtectVirtualMemory"));
    instrumentation::hooked.push_back(ntdll->GetProcAddress("NtQueryVirtualMemory"));
    instrumentation::hooked.push_back(ntdll->GetProcAddress("NtReadVirtualMemory"));
    instrumentation::hooked.push_back(ntdll->GetProcAddress("NtReadVirtualMemoryEx"));
    instrumentation::hooked.push_back(ntdll->GetProcAddress("NtOpenFile"));
    instrumentation::hooked.push_back(ntdll->GetProcAddress("NtGetContextThread"));
    #endif
}

// 48 89 4C 24 ? 48 81 EC ? ? ? ? 48 C7 44 24 - DllVerification
// 48 89 4C 24 ? 48 83 EC ? 48 8B 44 24 ? 48 83 78 ? ? 74 ? E9 - DllVerification2
// 44 88 4C 24 ? 4C 89 44 24 ? 48 89 54 24 ? 48 89 4C 24 ? B8 - CUserMessage_DllStatus


DWORD instrumentation::tls_index;

void callback(CONTEXT *ctx) {
    // by using the TLS, we can make sure to handle all syscalls coming from
    // all threads, and we can call syscalls ourselves inside the callback without
    // recursing forever.

    // Update these offsets to your windows version from ntdiff:
    // https://ntdiff.github.io/
    // /* 0x02d0 */ unsigned __int64 InstrumentationCallbackSp;
    // /* 0x02d8 */ unsigned __int64 InstrumentationCallbackPreviousPc;
    // /* 0x02e0 */ unsigned __int64 InstrumentationCallbackPreviousSp;

    // Grab the teb to extract instrumentation callback specific information.
    auto teb = reinterpret_cast<uint64_t>(NtCurrentTeb());

    // Grab and store the address we should return to.
    ctx->Rip = *reinterpret_cast<uint64_t *>(teb + 0x02d8);
    // Grab and store the stack pointer that we should restore.
    ctx->Rsp = *reinterpret_cast<uint64_t *>(teb + 0x02e0);
    // Recover original RCX.
    ctx->Rcx = ctx->R10;

    // First check if this thread is already handling a syscall, hence this coming
    // syscall is coming from inside the callback itself.
    if (instrumentation::is_thread_handling_syscall()) {
        // Abort if it's already handling a syscall, or if reading the data is not
        // successful.
        RtlRestoreContext(ctx, nullptr);
    }

    // Try setting the TLS variable to indicate that the thread is now handling a
    // syscall.
    if (!instrumentation::set_thread_handling_syscall(true)) {
        // Abort if the variable couldn't be set.
        RtlRestoreContext(ctx, nullptr);
    }

    // Grab the return address of the call.
    auto return_address = reinterpret_cast<void *>(ctx->Rip);
    // Grab the return value of the call.
    auto return_value = reinterpret_cast<void *>(ctx->Rax);
    // Variable that will hold teh amount of bytes into the function
    // the return address is located into.
    uint64_t offset_into_function;
    // Grab the function's name.
    auto function_name = syms::g_parser->get_function_sym_by_address(return_address, &offset_into_function);
    auto function_address = syms::g_parser->get_function_start(return_address);

    auto address = std::ranges::find(instrumentation::hooked, function_address);
    if (address == instrumentation::hooked.end())
    {
        // Restore the context back to the original one, continuing execution.
        RtlRestoreContext(ctx, nullptr);
    }

    auto ntdll = CMemory::GetModule("ntdll.dll");
    // instrumentation::hooked.push_back(ntdll->GetProcAddress("NtQueryInformationProcess"));
    //instrumentation::hooked.push_back(ntdll->GetProcAddress("NtQueryInformationThread"));

    // Print out all the information.
    std::cout << "[rax=0x" << return_value << "] "
              << "[rip=0x" << return_address << "] "
              << "syscall returning to " << function_name << "+0x" << std::hex << offset_into_function << " (" << function_name << ")"
              << std::endl;

    if (*address == ntdll->GetProcAddress("NtQueryInformationThread")) {
        // Print the arguments
      std::cout << "NtQueryInformationThread arguments: " << std::endl;
        std::cout << "ThreadHandle: 0x" << std::hex << ctx->Rcx << " | Own: 0x" << std::hex << curThread << std::endl;
      std::cout << "ThreadInformationClass: 0x" << std::hex << ctx->Rdx << std::endl;
      std::cout << "ThreadInformation: 0x" << std::hex << ctx->R8 << std::endl;
      std::cout << "ThreadInformationLength: 0x" << std::hex << ctx->R9 << std::endl;
      std::cout << "ReturnLength: 0x" << std::hex << ctx->R10 << std::endl;
    }

    // Unlock the thread so that it will be able to handle another syscall.
    instrumentation::set_thread_handling_syscall(false);
    // Restore the context back to the original one, continuing execution.
    RtlRestoreContext(ctx, nullptr);
}

bool *instrumentation::get_thread_data_pointer() {
    void *thread_data = nullptr;
    bool *data_pointer = nullptr;

    thread_data = TlsGetValue(instrumentation::tls_index);

    if (thread_data == nullptr) {
        thread_data = reinterpret_cast<void *>(LocalAlloc(LPTR, 256));

        if (thread_data == nullptr) {
            return nullptr;
        }

        RtlZeroMemory(thread_data, 256);

        if (!TlsSetValue(instrumentation::tls_index, thread_data)) {
            return nullptr;
        }
    }

    data_pointer = reinterpret_cast<bool *>(thread_data);

    return data_pointer;
}

bool instrumentation::set_thread_handling_syscall(bool value) {
    if (auto data_pointer = get_thread_data_pointer()) {
        *data_pointer = value;
        return true;
    }

    return false;
}

bool instrumentation::is_thread_handling_syscall() {
    if (auto data_pointer = get_thread_data_pointer()) {
        return *data_pointer;
    }

    return false;
}

bool instrumentation::initialize() {
    auto ntdll = CMemory::GetModule("ntdll.dll");

    auto nt_set_information_process =
        reinterpret_cast<instrumentation::nt_set_information_process_t>(ntdll->GetProcAddress("NtSetInformationProcess"));

    if (!nt_set_information_process) {
        std::cout << "[+] Couldn't gather address for NtSetInformationProcess" << std::endl;
        return false;
    }

    instrumentation::tls_index = TlsAlloc();

    if (instrumentation::tls_index == TLS_OUT_OF_INDEXES) {
        std::cout << "Couldn't allocate a TLS index" << std::endl;

        return false;
    }

    process_instrumentation_callback_info_t info;
    info.version = 0;  // x64 mode
    info.reserved = 0;
    info.callback = bridge;

    nt_set_information_process(GetCurrentProcess(), static_cast<PROCESS_INFORMATION_CLASS>(0x28), &info, sizeof(info));

    return true;
}

void instrumentation::shutdown() {
    auto ntdll = CMemory::GetModule("ntdll.dll");

    auto nt_set_information_process =
        reinterpret_cast<instrumentation::nt_set_information_process_t>(ntdll->GetProcAddress("NtSetInformationProcess"));

    if (!nt_set_information_process) {
        std::cout << "[+] Couldn't gather address for NtSetInformationProcess" << std::endl;
        return;
    }

    process_instrumentation_callback_info_t info;
    info.version = 0;  // x64 mode
    info.reserved = 0;
    info.callback = nullptr;

    nt_set_information_process(GetCurrentProcess(), static_cast<PROCESS_INFORMATION_CLASS>(0x28), &info, sizeof(info));
}
