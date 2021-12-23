#include "Trainer.h"
Trainer* trainer = nullptr;
Memory::TrampolineHook* hook = nullptr;

int __stdcall wglSwapBuffersHooked(HDC hdc) {
    auto fn = (wglSwapBuffersFunction)hook->Gateway();
    if (hook->IsEnabled() && trainer != nullptr)
    {
        trainer->Tick(hdc);
        return fn(hdc);
    }
    return fn(hdc);
}

DWORD WINAPI TrainerThread(HMODULE hModule)
{
    trainer = new Trainer(hModule);

    // Hook opengl event loop
    uintptr_t wglSwapBuffers = (uintptr_t)GetProcAddress(GetModuleHandle(L"opengl32.dll"), "wglSwapBuffers");
    hook = new Memory::TrampolineHook(wglSwapBuffers, wglSwapBuffersHooked, 5);
    hook->Enable();

    // Wait for exit event
    while (!(GetAsyncKeyState(VK_DELETE) & 1))
    {
        Sleep(10);
    }

    // Stop trainer
    hook->Disable();
    delete trainer;
    trainer = nullptr;
    FreeLibraryAndExitThread(hModule, 0);
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

