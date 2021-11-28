#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <iostream>
#include <Windows.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>

class Trainer
{
private:
	WNDCLASSEX wc;
	HWND hwnd;

	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;
public:
		Trainer(HMODULE hModule);
		bool Tick();
		void Cleanup();
};

