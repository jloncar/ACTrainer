#include "Game.h"

Game::Engine::Engine() : baseAddress(NULL), player(NULL), entities(NULL)
{
	baseAddress = (uintptr_t)GetModuleHandle(L"ac_client.exe");
	if (baseAddress == NULL)
	{
		return;
	}

	player = *reinterpret_cast<Player**>(baseAddress + 0x10F4F4);
	entities = new EntityList(baseAddress);
}

Game::Engine::~Engine()
{
	delete entities;
}

Game::EntityList::EntityList(uintptr_t moduleBaseAddress)
{
	baseAddress = reinterpret_cast<uintptr_t*>(moduleBaseAddress + 0x10F4F8);
	count = reinterpret_cast<int*>(moduleBaseAddress + 0x10F500);
}

std::vector<Game::Player*> Game::EntityList::All()
{
	std::vector<Game::Player*> result;

	if (*count <= 1)
		return result;

	// count-1: Count includes localPlayer, which is doesn't have pointer in this struct
	// count-2: i is start from 0
	// 0x4 is some wierd padding because AC is cool
	for (int i = 0; i < (*count - 2); i++)
	{
		result.push_back(*reinterpret_cast<Game::Player**>(*baseAddress + 0x4 + i * sizeof(Game::Player**)));
	}
	return result;
}