#include "Trainer.h"


// Hook opengl functions opengl32.dll::wglSwapBuffers (exported function)
// 8B FF 55 8B EC
// .text:1A775E21                 mov     edi, edi
// .text:1A775E23                 push    ebp
// .text:1A775E24                 mov     ebp, esp
// https://stackoverflow.com/a/4830888/531212
typedef int(__stdcall* wglSwapBuffersFunction)(HDC hDC);
wglSwapBuffersFunction gateway;

static int cnt = 0;
int __stdcall wglSwapBuffersHooked(HDC hDC) {
    _RPT1(0, "Hello World: %d\r", ++cnt);
    // https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-wglgetcurrentdc

    return gateway(hDC);
}


DWORD WINAPI TrainerThread(HMODULE hModule)
{
    uintptr_t wglSwapBuffers = (uintptr_t)GetProcAddress(GetModuleHandle(L"opengl32.dll"), "wglSwapBuffers");
    gateway = (wglSwapBuffersFunction)Memory::TrampolineHook(wglSwapBuffers, wglSwapBuffersHooked, 5);


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

