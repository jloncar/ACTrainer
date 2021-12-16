#pragma once
#include <Windows.h>
#include "Memory.h"

struct LocalPlayer {
	uintptr_t p_baseModule;
	uintptr_t p_localPlayer;

	LocalPlayer();

	int* hp;
	int* ammo;

	void AddGranade();
	bool PatchInvulnerableEveryone(bool enabled);
	bool PatchInvulnerableSelf(bool enabled);
};