#pragma once
#pragma comment(lib, "OpenGL32.lib")

#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <vector>
#include <algorithm>
#include <gl/GL.h>
#include "Common.h"
#include "Overlay.h"
#include "Memory.h"
#include "LocalPlayer.h"

typedef int(__stdcall* wglSwapBuffersFunction)(HDC hDC);

using namespace Features;

class Trainer
{
typedef void(*FeatureCallback)(const Trainer* trainer, Feature* feature, LocalPlayer* player);
private:
	Features::Collection* m_Features;
	std::unordered_map<Features::Feature*, FeatureCallback> m_FeatureCallbacks;

	Overlay* m_Overlay;
	HWND m_GameWindow;

	void AddFeature(std::string name, Features::Feature::Widget widget, Features::Feature::Group group, FeatureCallback callback);

	// OpenGL hooking
	wglSwapBuffersFunction m_Gateway;
	bool m_ContextCreated = false;
	HGLRC m_ContextTrainer;
	HGLRC m_ContextGame;
	void SetOpenGLContext(HDC hdc);

public:
		Trainer(HMODULE& hModule, const wchar_t* gameWindowTitle, wglSwapBuffersFunction eventLoop);
		int Tick(HDC hdc);
		~Trainer();
};

