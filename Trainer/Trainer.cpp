#include "pch.h"
#include "Trainer.h"

Trainer::Trainer(HMODULE hModule) {
	// Setup output window
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "Hello World!" << std::endl;


	// Init variables
	b_infiniteHealth = false;
	cnt = 0;
}
	
bool Trainer::Tick() {
	std::cout << "Loop: " << ++cnt << "\r";
	// Returning false will stop event loop
	return true;
}
