#pragma once
#include <Windows.h>

struct LocalPlayer {
	uintptr_t p_baseModule;
	uintptr_t p_localPlayer;

	LocalPlayer();

	int* hp;


};