#pragma once
#define WIN32_LEAN_AND_MEAN
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include "Common.h"
#include "dwmapi.h"

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


class Overlay
{
public:
	void Init(Config* config, HMODULE& hModule, HWND& parent);
	bool Tick();
	void Cleanup();

private:
	WNDCLASSEX m_WinClass;
	HWND m_Window;
	Config* m_Config;

	void CreateAndBindWindow(const wchar_t* title, HMODULE& hModule, HWND& parentWindow);
	void HookNavigation();
};

