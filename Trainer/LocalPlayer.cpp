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

	// Resolve variables
	
	hp = reinterpret_cast<int*>(p_localPlayer + 0xF8);

	std::vector<int> ammoOffsets  = { 0x374 , 0x14 , 0x0 };
	ammo = reinterpret_cast<int*>(Memory::ResolvePtr(p_localPlayer, ammoOffsets));

}


bool LocalPlayer::PatchInvulnerableEveryone(bool enabled)
{
	const size_t sequenceSize = 3;
	byte sequence[sequenceSize] = { 0x29, 0x7B, 0x04};
	byte newSequence[sequenceSize] = { 0x90, 0x90, 0x90 };

	static uintptr_t sequencePtr = (sequencePtr == NULL) ? Memory::FindSequence(L"ac_client.exe", sequence, sequenceSize) : sequencePtr;
	
	if (enabled)
	{
		Memory::PatchSequence(sequencePtr, newSequence, sequenceSize);
	}
	else {
		Memory::PatchSequence(sequencePtr, sequence, sequenceSize);
	}

	return true;
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
	asm_localPlayerAddress = p_localPlayer;
	// Resume take dmg addy
	asm_jmpBkInvulnerableSelf = p_baseModule + 0x269F0 + sequenceSize;

	if (enabled)
	{
		Memory::PatchJump(start, InvulnerableSelfHook, sequenceSize);
	}
	else {
		Memory::PatchSequence(start, originalSequence, sequenceSize);
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

	// Has to be called from asm because __usercall
	__asm {
		mov eax, playerPtr
		mov ecx, ammoType
		call addAmmoAddress
	}

}