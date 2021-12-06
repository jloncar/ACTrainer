#include "LocalPlayer.h"

LocalPlayer::LocalPlayer() : p_baseModule(NULL), p_localPlayer(NULL), ammo(nullptr), hp(nullptr)
{
	// If both are set, nothing to initialize, skip for optimization
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


void LocalPlayer::PatchSequence(uintptr_t start, const char* newSequence)
{
	DWORD lpflOldProtectVal;
	const unsigned int sequenceSize = strlen(newSequence);

	if (!VirtualProtect((void*)start, sequenceSize, PAGE_EXECUTE_READWRITE, &lpflOldProtectVal))
	{
		// handle error
		return;
	}

	for (uintptr_t iterator = start; iterator <= start + sequenceSize; iterator++)
	{
		*(reinterpret_cast<char*>(iterator)) = newSequence[(iterator-start)];
	}
}

uintptr_t LocalPlayer::FindSequence(const char* sequence)
{
	HMODULE hModule = GetModuleHandle(L"ac_client.exe");
	MODULEINFO moduleInfo;

	GetModuleInformation(
		GetCurrentProcess(),
		hModule,
		&moduleInfo,
		sizeof(MODULEINFO)
	);

	const uintptr_t endAddress = p_baseModule + moduleInfo.SizeOfImage;
	uintptr_t sequenceAddr = NULL;

	for (uintptr_t iterator = p_baseModule; iterator < (p_baseModule + moduleInfo.SizeOfImage); iterator++)
	{

		bool matching = false;

		for (int seqidx = 0; seqidx < strlen(sequence); seqidx++)
		{
			char byte = *reinterpret_cast<char*>(iterator + seqidx);

			if (byte != sequence[seqidx] && sequence[seqidx] != '?')
			{
				matching = false;
				break;
			}

			matching = true;
		}

		if (matching)
		{
			sequenceAddr = iterator;
			break;
		}
	}

	return sequenceAddr;
}

bool LocalPlayer::PatchInvulnerable(bool enabled)
{
	const char* sequence = "\x29\x7B\x04";
	static uintptr_t sequencePtr = (sequencePtr == NULL) ? FindSequence(sequence) : sequencePtr;
	
	if (enabled)
	{
		PatchSequence(sequencePtr, "\x90\x90\x90");
	}
	else {
		PatchSequence(sequencePtr, sequence);
	}

	return true;
}