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
#include "Game.h"

typedef int(__stdcall* wglSwapBuffersFunction)(HDC hDC);

using namespace Features;

class Trainer
{
typedef void(*FeatureCallback)(const Trainer* trainer, Feature* feature, Game::Engine* game);
private:
	Features::Collection* m_Features;
	std::unordered_map<Features::Feature*, FeatureCallback> m_FeatureCallbacks;

	Game::Engine* m_Game;
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
		Trainer(HMODULE& hModule, wglSwapBuffersFunction eventLoop);
		int Tick(HDC hdc);
		~Trainer();
};

