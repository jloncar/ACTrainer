#pragma once
#include <Windows.h>

DWORD asm_localPlayerAddress;
DWORD asm_jmpBkInvulnerableSelf;


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

		TAKE_DMG:
		// Overwritten bytes (6)
		push ebp
		mov ebp, esp
		and esp, 0FFFFFFF8h
		// Jump back, absolute jump 0xFF
		jmp asm_jmpBkInvulnerableSelf
	}
}