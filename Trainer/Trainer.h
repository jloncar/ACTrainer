#pragma once

#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <vector>
#include <algorithm>
#include "Common.h"
#include "Overlay.h"
#include "Memory.h"
#include "LocalPlayer.h"

enum Patches {
	INVULNERABLE_EVERYONE
};

class Trainer
{
private:
	Config* m_Config;
	Overlay* m_Overlay;
	HWND m_GameWindow;
	std::vector<Patches> m_PatchesApplied;

public:
		Trainer(HMODULE& hModule, const wchar_t* gameWindowTitle);
		bool Tick();
		~Trainer();
};

