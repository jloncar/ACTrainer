#include "Overlay.h"
#include "Trainer.h"

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

void Overlay::CreateAndBindWindow(const wchar_t* title, HMODULE& hModule, HWND& parentWindow)
{
    m_WinClass = { sizeof(WNDCLASSEX), ACS_TRANSPARENT, WndProc, 0L, 0L, hModule, NULL, NULL, NULL, NULL, title, NULL };
    ::RegisterClassEx(&m_WinClass);
    m_Window = ::CreateWindow(m_WinClass.lpszClassName, title, WS_POPUP | WS_EX_TOPMOST | WS_EX_TRANSPARENT, 0, 0, 500, 500, parentWindow, NULL, m_WinClass.hInstance, NULL);
    SetLayeredWindowAttributes(m_Window, 0, 1.0f, LWA_ALPHA);
    SetLayeredWindowAttributes(m_Window, 0, RGB(0, 0, 0), LWA_COLORKEY);
    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(m_Window, &margins);
}

void Overlay::Init(Config* config, HMODULE& hModule, HWND& parentWindow)
{
    // Set state
    m_Config = config;
    this->CreateAndBindWindow(L"Trainer", hModule, parentWindow);

    // Initialize Direct3D
    if (!CreateDeviceD3D(m_Window))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(m_WinClass.lpszClassName, m_WinClass.hInstance);
        return;
    }

    // Show the window
    ::ShowWindow(m_Window, SW_SHOWDEFAULT);
    ::UpdateWindow(m_Window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(m_Window);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
}

void Overlay::HookNavigation() {
    ImGuiIO& io = ImGui::GetIO();

    io.KeysDown[VK_DOWN] = GetAsyncKeyState(VK_DOWN);
    io.KeysDown[VK_UP] = GetAsyncKeyState(VK_UP);
    io.KeysDown[VK_SPACE] = GetAsyncKeyState(VK_RETURN);
}

bool Overlay::Tick() {

    this->HookNavigation();

    MSG msg;
    while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            return false;
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    static int corner = 0;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse;
    if (corner != -1)
    {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    // ImGui::ShowDemoWindow();
    ImGui::SetNextWindowBgAlpha(0.9f); // Transparent background
    if (ImGui::Begin("Trainer", NULL, window_flags))
    {
        ImGui::Text("Use keyboard to navigate");
        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Core", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("Freeze HP", &m_Config->b_freezeHp);
            ImGui::Checkbox("Infinite ammo", &m_Config->b_infiniteAmmo);
            ImGui::Checkbox("Invulnerable", &m_Config->b_invulnerable);
        }
        if (ImGui::CollapsingHeader("Advanced", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("No Recoil", &m_Config->b_noRecoil);
            ImGui::Checkbox("Aimbot", &m_Config->b_aimbot);
        }
        
    }
    ImGui::End();

    // Rendering
    const float clear_color_with_alpha[4] = { .0f, .0f, .0f, .0f };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    g_pSwapChain->Present(1, 0); // Present with vsync

    return true;
}

void Overlay::Cleanup()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(m_Window);
    ::UnregisterClass(m_WinClass.lpszClassName, m_WinClass.hInstance);

}

// Helper functions
bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}


// Enables imgui to recieve events such as mouse clicks/keyboard and react to it
// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
