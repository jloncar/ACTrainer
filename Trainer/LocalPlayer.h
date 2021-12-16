#pragma once
#include <Windows.h>
#include "Memory.h"

struct LocalPlayer {
private:
	uintptr_t p_baseModule;
	uintptr_t p_localPlayer;

public:
	LocalPlayer();

	int* hp;
	int* ammo;

	void AddGranade();
	bool PatchInvulnerableEveryone(bool enabled);
	bool PatchInvulnerableSelf(bool enabled);
};