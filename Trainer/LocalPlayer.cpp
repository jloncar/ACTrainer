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
}
