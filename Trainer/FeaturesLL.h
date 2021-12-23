#pragma once
// Lower level feature functions, naked functions etc.

uint32_t asm_localPlayerAddress;
uint32_t asm_jmpBkInvulnerableSelf;

// REPLACES:
// "playerHit": 
// ac_client.exe + 269F0
// 55 8B EC 83 E4 F8
// .text:004269F0                 push    ebp
// .text:004269F1                 mov     ebp, esp
// .text:004269F3				  and esp, 0FFFFFFF8h
// int __usercall playerHit@<eax>(int target@<eax>, int damage, int *attacker, int a4, char a5, char a6)
void __declspec(naked) InvulnerableSelfHook()
{
	__asm {
		// Check if target is local player; if it is - don't take dmg.
		cmp eax, asm_localPlayerAddress
		jne TAKE_DMG
		retn

		TAKE_DMG :
		// Overwritten bytes (6)
		push ebp
			mov ebp, esp
			and esp, 0FFFFFFF8h
			// Jump back, absolute jump 0xFF
			jmp asm_jmpBkInvulnerableSelf
	}
}


bool PatchInvulnerableEveryone(bool enabled)
{
	const size_t sequenceSize = 3;
	byte sequence[sequenceSize] = { 0x29, 0x7B, 0x04 };
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


bool PatchInvulnerableSelf(bool enabled, Game::Engine* game)
{
	// Size of bytes of instruction set we're overwriting.
	// JMP requires minimum of 5 bytes
	const size_t sequenceSize = 6;

	byte originalSequence[sequenceSize] = { 0x55, 0x8B, 0xEC, 0x83, 0xE4, 0xF8 };

	// Replace with pattern scan if needed
	uintptr_t start = game->baseAddress + 0x269F0;

	// Configure vars used in ASM
	asm_localPlayerAddress = (uint32_t)game->player;

	// Resume take dmg addy
	asm_jmpBkInvulnerableSelf = game->baseAddress + 0x269F0 + sequenceSize;

	if (enabled)
	{
		Memory::PatchJump(start, InvulnerableSelfHook, sequenceSize);
	}
	else {
		Memory::PatchSequence(start, originalSequence, sequenceSize);
	}

	return true;
}

void AddGranade(Game::Engine* game)
{
	// (p_baseModule + 0x466B0)

	/// int __usercall addAmmo@<eax>(int playerPtr@<eax>, int a2@<ecx>)

	//int(*addAmmo)(int, int) = (int (*)(int,int))(p_baseModule + 0x466B0);	
	//int addAmmoRes = addAmmo(p_localPlayer + 0xF4, 0x5);

	DWORD playerPtr = (uint32_t)game->player + 0xF4;
	unsigned int ammoType = 0x5;
	DWORD addAmmoAddress = game->baseAddress + 0x466B0;

	// Has to be called from asm because __usercall
	__asm {
		mov eax, playerPtr
		mov ecx, ammoType
		call addAmmoAddress
	}

}


void DrawESPBox()
{
	//ImGui::GetForegroundDrawList()->AddCircle(ImVec2(100, 100), 100 * 0.6f, IM_COL32(0, 255, 0, 200), 0, 10);
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(100, 100), ImVec2(200, 200), IM_COL32(0, 255, 0, 200), 0, 0, 10);
}