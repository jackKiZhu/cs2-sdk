#include <pch.hpp>

#include <instance/instance.hpp>
#include <logger/logger.hpp>
#include <memory/memory.hpp>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);

        CMemory::SetSelf(hinstDLL);

        HANDLE hThread = CreateThread(
            NULL, 0,
            [](LPVOID pData) -> DWORD {
                CLogger::Get().Initialize();
                CInstance::Get().Initialize();

                return 0;
            },
            hinstDLL, 0, NULL);

        if (hThread != NULL) {
            CloseHandle(hThread);
        }
    } else if (fdwReason == DLL_PROCESS_DETACH && !lpvReserved) {
        CInstance::Get().Shutdown();
    }

    return TRUE;
}
