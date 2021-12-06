#pragma once
#include <Windows.h>
#include <vector>
#include <Psapi.h>

struct LocalPlayer {
	uintptr_t p_baseModule;
	uintptr_t p_localPlayer;

	LocalPlayer();

	int* hp;
	int* ammo;

	// @todo move to memory mgmt module
	uintptr_t ResolvePtr(std::vector<int> offsets);
	uintptr_t FindSequence(const char* sequence);
	void PatchSequence(uintptr_t start, const char* newSequence);

	bool PatchInvulnerable(bool enabled);

};