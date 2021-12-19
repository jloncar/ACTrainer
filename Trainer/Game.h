#pragma once
#include <Windows.h>
#include "Memory.h"

struct Game {

	uintptr_t p_baseModule;

	struct LocalPlayer {
		Game* p_game;
		uintptr_t p_localPlayer;
		int* hp;
		int* ammo;

		LocalPlayer(Game* game, uintptr_t address);
		void AddGranade();
	};

	LocalPlayer* localPlayer;
	Game();

	bool PatchInvulnerableEveryone(bool enabled);
	bool PatchInvulnerableSelf(bool enabled);
};
