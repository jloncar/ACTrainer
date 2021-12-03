#include "Trainer.h"

DWORD WINAPI TrainerThread(HMODULE hModule)
{
    Trainer* t = new Trainer(hModule, L"AssaultCube");

    while (t->Tick())
    {
        continue;
    }

    delete t;

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

