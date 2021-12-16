#include "LocalPlayer.h"
#include "Naked.h"

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


void LocalPlayer::PatchSequence(uintptr_t start, byte* newSequence, unsigned int sequenceSize)
{
	DWORD lpflOldProtectVal;

	if (!VirtualProtect((void*)start, sequenceSize, PAGE_EXECUTE_READWRITE, &lpflOldProtectVal))
	{
		// handle error
		return;
	}

	for (uintptr_t iterator = start; iterator < start + sequenceSize; iterator++)
	{
		*(reinterpret_cast<char*>(iterator)) = newSequence[(iterator-start)];
	}

	// TODO Restore VirtualProtect
}

uintptr_t LocalPlayer::FindSequence(byte* sequence, unsigned int sequenceSize)
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

		for (int seqidx = 0; seqidx < sequenceSize; seqidx++)
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

bool LocalPlayer::PatchInvulnerableEveryone(bool enabled)
{
	const size_t sequenceSize = 3;
	byte sequence[sequenceSize] = { 0x29, 0x7B, 0x04};
	byte newSequence[sequenceSize] = { 0x90, 0x90, 0x90 };

	static uintptr_t sequencePtr = (sequencePtr == NULL) ? FindSequence(sequence, sequenceSize) : sequencePtr;
	
	if (enabled)
	{
		PatchSequence(sequencePtr, newSequence, sequenceSize);
	}
	else {
		PatchSequence(sequencePtr, sequence, sequenceSize);
	}

	return true;
}

void LocalPlayer::PatchJump(uintptr_t start, void* hookFunction, const size_t sequenceSize)
{
	// Allocate at least 5 bytes on stack; first byte = JMP, next 4 - target hook addy, rest: nop
	// pointer is cast to uintptr_t* to defer conversion to compiler - otherwise we'd need to manually populate each offset +1 +2 +3 +4 using bitshift
	// But then endinaness is important... Easier to leave it to compiler.
	byte* sequence = (byte*)alloca(sequenceSize * sizeof(byte));
	memset(sequence, 0x90, sequenceSize);

	// jmp (0xE9) - relative jump
	*sequence = 0xE9;

	// targetOffset = targetLocation - jumpLocation - jumpSize
	*(uintptr_t*)(sequence + 1) = (uintptr_t)hookFunction - (p_baseModule + 0x269F0) - 5;

	PatchSequence(p_baseModule + 0x269F0, sequence, sequenceSize);
}

bool LocalPlayer::PatchInvulnerableSelf(bool enabled)
{
	// Size of bytes of instruction set we're overwriting.
	// JMP requires minimum of 5 bytes
	const size_t sequenceSize = 6;

	byte originalSequence[sequenceSize] = { 0x55, 0x8B, 0xEC, 0x83, 0xE4, 0xF8 };

	// Replace with pattern scan if needed
	uintptr_t start = p_baseModule + 0x269F0;

	// Configure vars used in ASM
	localPlayerAddy = p_localPlayer;
	// Resume take dmg addy
	jmpBkInvulnerableSelf = p_baseModule + 0x269F0 + sequenceSize;

	if (enabled)
	{
		PatchJump(start, InvulnerableSelfHook, sequenceSize);
	}
	else {
		PatchSequence(start, originalSequence, sequenceSize);
	}
	
	return true;
}


void LocalPlayer::AddGranade()
{
	// (p_baseModule + 0x466B0)

	/// int __usercall addAmmo@<eax>(int playerPtr@<eax>, int a2@<ecx>)

	//int(*addAmmo)(int, int) = (int (*)(int,int))(p_baseModule + 0x466B0);	
	//int addAmmoRes = addAmmo(p_localPlayer + 0xF4, 0x5);

	DWORD playerPtr = p_localPlayer + 0xF4;
	unsigned int ammoType = 0x5;
	DWORD addAmmoAddress = p_baseModule + 0x466B0;

	__asm {
		mov eax, playerPtr
		mov ecx, ammoType
		call addAmmoAddress
	}

}