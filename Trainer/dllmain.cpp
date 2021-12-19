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
bool contextCreated = false;
HGLRC myContext;
HGLRC gameContext;

int __stdcall wglSwapBuffersHooked(HDC hdc) {
    _RPT1(0, "Hello World: %d\r", ++cnt);
    // https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-wglgetcurrentdc
    // https://guidedhacking.com/threads/how-to-get-started-with-opengl-hacks.11475/post-98942
        //Save the games context
    gameContext = wglGetCurrentContext();

    //Create our own context if it hasn't been created yet
    if (contextCreated == false)
    {
        //Create new context
        myContext = wglCreateContext(hdc);

        //Make thread use our context
        wglMakeCurrent(hdc, myContext);

        //Setup our context
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        //Get resolution
        GLint m_viewport[4];
        glGetIntegerv(GL_VIEWPORT, m_viewport);

        glOrtho(0.0, m_viewport[2], m_viewport[3], 0.0, 1.0, -1.0);  //might want to make these your actual screen resolution
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClearColor(0, 0, 0, 1.0);
        contextCreated = true;
    }

    //Make thread use our context
    wglMakeCurrent(hdc, myContext);

    //Draw something (a rectangle here)
    glColor3f(1.0f, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(0, 190.0f);
    glVertex2f(100.0f, 190.0f);
    glVertex2f(100.0f, 290.0f);
    glVertex2f(0, 290.0f);
    glEnd();

    //Make thread to use games context again
    wglMakeCurrent(hdc, gameContext);

    //return to orginal swapbuffers

    return gateway(hdc);
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

