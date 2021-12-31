#pragma once
#pragma comment(lib, "OpenGL32.lib")

#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <libloaderapi.h>
#include <vector>
#include <algorithm>
#include <gl/GL.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
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
	bool m_ContextCreated = false;
	HGLRC m_ContextTrainer;
	HGLRC m_ContextGame;
	void SetOpenGLContext(HDC hdc);

public:
		Trainer(HMODULE& hModule);
		void Tick(HDC hdc);
		~Trainer();
};

