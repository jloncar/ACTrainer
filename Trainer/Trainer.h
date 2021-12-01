#pragma once

#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include "Common.h"
#include "Overlay.h"

class Trainer
{
private:
	Config* m_Config;
	Overlay* m_Overlay;
public:
		Trainer(HMODULE& hModule);
		bool Tick();
		~Trainer();

};

