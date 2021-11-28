#include "pch.h"
#include "Trainer.h"

// Debugging DLL:
// Right click DLL project, Debug, Step into new Instance
// Make sure debugging props (attach, emit debug info) are set properly
// https://docs.microsoft.com/en-us/visualstudio/debugger/debugging-dll-projects?view=vs-2022
// https://docs.microsoft.com/en-us/visualstudio/debugger/debug-multiple-processes?view=vs-2022

DWORD WINAPI TrainerThread(HMODULE hModule)
{
    Trainer t{ hModule };

    while (t.Tick())
    {
        Sleep(100);
    }

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)TrainerThread, hModule, 0, 0));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

