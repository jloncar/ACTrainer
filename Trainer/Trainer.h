#pragma once

#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include "Common.h"
#include "Overlay.h"
#include "LocalPlayer.h"

class Trainer
{
private:
	Config* m_Config;
	Overlay* m_Overlay;
	HWND m_GameWindow;

public:
		Trainer(HMODULE& hModule, const wchar_t* gameWindowTitle);
		bool Tick();
		~Trainer();
};

