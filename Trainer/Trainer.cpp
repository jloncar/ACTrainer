#include "Trainer.h"
#include "Features.h"

Trainer::Trainer(HMODULE& hModule, wglSwapBuffersFunction eventLoop)
{
    m_GameWindow = FindWindow(NULL, L"AssaultCube");

    // Hook opengl event loop
    uintptr_t wglSwapBuffers = (uintptr_t)GetProcAddress(GetModuleHandle(L"opengl32.dll"), "wglSwapBuffers");
    m_Gateway = (wglSwapBuffersFunction)Memory::TrampolineHook(wglSwapBuffers, eventLoop, 5);

    // Add features
    m_Features = new Features::Collection();
    AddFeature("Freeze Ammo", Features::Feature::Widget::Checkbox, Features::Feature::Group::Core, (FeatureCallback)FreezeAmmo);
    AddFeature("ESP", Features::Feature::Widget::Checkbox, Features::Feature::Group::Core, (FeatureCallback)ESP);

    AddFeature("Freeze Health", Features::Feature::Widget::Checkbox, Features::Feature::Group::BotOnly, (FeatureCallback)FreezeHealth);
    AddFeature("Invulnerability (all)", Features::Feature::Widget::Checkbox, Features::Feature::Group::BotOnly, (FeatureCallback)InvulnerableEveryone);
    AddFeature("Invulnerability (self)", Features::Feature::Widget::Checkbox, Features::Feature::Group::BotOnly, (FeatureCallback)InvulnerableSelf);

    // Setup state
    m_Game = new Game::Engine();
    m_Overlay = new Overlay(m_Features, m_GameWindow);
}


void Trainer::AddFeature(std::string name, Features::Feature::Widget widget, Features::Feature::Group group, FeatureCallback callback) {
    Features::Feature* f = m_Features->Add(name, widget, group);
    m_FeatureCallbacks.insert(std::make_pair(f, callback));
}


int Trainer::Tick(HDC hdc) {
    SetOpenGLContext(hdc);


    m_Overlay->HookNavigation();
    m_Overlay->StartFrame();
    for (auto feature : m_Features->All())
    {
        try {
            FeatureCallback callback = m_FeatureCallbacks.at(feature);
            callback(this, feature, m_Game);
        } catch (const std::out_of_range& oor) {
            OutputDebugString(L"Unable to find feature.");
            continue;
        }
    }
    m_Overlay->Menu();
    m_Overlay->RenderFrame();


    //Make thread to use games context again
    wglMakeCurrent(hdc, m_ContextGame);

    return m_Gateway(hdc);
}


// Hook opengl functions opengl32.dll::wglSwapBuffers (exported function)
// 8B FF 55 8B EC
// .text:1A775E21                 mov     edi, edi
// .text:1A775E23                 push    ebp
// .text:1A775E24                 mov     ebp, esp
// https://stackoverflow.com/a/4830888/531212
// https://guidedhacking.com/threads/how-to-get-started-with-opengl-hacks.11475/post-98942
void Trainer::SetOpenGLContext(HDC hdc)
{

    //Save the games context
    m_ContextGame = wglGetCurrentContext();

    //Create our own context if it hasn't been created yet
    if (m_ContextCreated == false)
    {
        //Create new context
        m_ContextTrainer = wglCreateContext(hdc);

        //Make thread use our context
        wglMakeCurrent(hdc, m_ContextTrainer);

        //Setup our context
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        //Get resolution
        GLint m_viewport[4];
        glGetIntegerv(GL_VIEWPORT, m_viewport);

        glOrtho(0.0, m_viewport[2], m_viewport[3], 0.0, 1.0, -1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClearColor(0, 0, 0, 1.0);
        m_ContextCreated = true;
    }

    //Make thread use our context
    wglMakeCurrent(hdc, m_ContextTrainer);
}

Trainer::~Trainer()
{
    delete m_Features;
    delete m_Overlay;
}
