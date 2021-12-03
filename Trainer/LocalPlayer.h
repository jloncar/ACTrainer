#pragma once
#include <Windows.h>
#include <vector>

struct LocalPlayer {
	uintptr_t p_baseModule;
	uintptr_t p_localPlayer;

	LocalPlayer();

	int* hp;
	int* ammo;

	uintptr_t ResolvePtr(std::vector<int> offsets);
};