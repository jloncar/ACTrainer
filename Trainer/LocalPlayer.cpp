#include "LocalPlayer.h"

LocalPlayer::LocalPlayer() : p_baseModule(NULL), p_localPlayer(NULL)
{
	if (p_baseModule != NULL && p_localPlayer != NULL)
	{
		return;
	}

	p_baseModule = (uintptr_t)GetModuleHandle(L"ac_client.exe");
	if (p_baseModule == NULL)
	{
		return;
	}

	p_localPlayer = *reinterpret_cast<uintptr_t*>(p_baseModule + 0x10F4F4);

	if (p_localPlayer == NULL)
	{
		return;
	}

	hp = reinterpret_cast<int*>(p_localPlayer + 0xF8);

	std::vector<int> ammoOffsets  = { 0x374 , 0x14 , 0x0 };
	ammo = reinterpret_cast<int*>(this->ResolvePtr(ammoOffsets));

}


uintptr_t LocalPlayer::ResolvePtr(std::vector<int> offsets)
{

	uintptr_t lvl  = p_localPlayer;
	int size = offsets.size();

	for (int i = 0; i < size; i++)
	{
		lvl = (*reinterpret_cast<uintptr_t*>(lvl + offsets[i]));

		// No need to resolve last pointer
		if ((i + 2) == size)
		{
			return lvl + offsets[size - 1];
		}
	}
}