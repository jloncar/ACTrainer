#pragma once
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <vector>
#include <algorithm>
#include <gl/GL.h>
#include "Common.h"
#include "dwmapi.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl2.h"

class Overlay
{
private:
	HWND m_Window;
	Features::Collection* m_Features;

	const char* ResolveGroupName(Features::Feature::Group group);

public:
	Overlay(Features::Collection* features, HWND& parentWindow);

	void Render(Features::Feature* feature);
	void Menu();
	void StartFrame();
	void RenderFrame();
	void HookNavigation();
	~Overlay();
};

