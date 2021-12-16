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

	void AddGranade();
	bool PatchInvulnerableEveryone(bool enabled);
	bool PatchInvulnerableSelf(bool enabled);

	// @todo move to memory mgmt module
	uintptr_t ResolvePtr(std::vector<int> offsets);
	uintptr_t FindSequence(byte* sequence, unsigned int sequenceSize);
	void PatchSequence(uintptr_t start, byte* newSequence, unsigned int sequenceSize);
	void PatchJump(uintptr_t start, void* hookFunction, size_t sequenceSize);

};