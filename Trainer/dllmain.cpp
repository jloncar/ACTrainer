#include "Trainer.h"
Trainer* trainer;

int __stdcall wglSwapBuffersHooked(HDC hdc) {
    return trainer->Tick(hdc);
}

DWORD WINAPI TrainerThread(HMODULE hModule)
{
    trainer = new Trainer(hModule, L"AssaultCube", wglSwapBuffersHooked);
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

