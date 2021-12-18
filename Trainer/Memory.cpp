#include "Memory.h"

uintptr_t Memory::ResolvePtr(uintptr_t base, std::vector<int> offsets)
{

	uintptr_t lvl = base;
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


void Memory::PatchSequence(uintptr_t start, byte* newSequence, unsigned int sequenceSize, byte* oldSequence)
{
	DWORD lpflOldProtectVal;

	if (!VirtualProtect((void*)start, sequenceSize, PAGE_EXECUTE_READWRITE, &lpflOldProtectVal))
	{
		// handle error
		return;
	}

	for (uintptr_t iterator = start; iterator < start + sequenceSize; iterator++)
	{
		if (oldSequence != nullptr)
			oldSequence[(iterator - start)] = *(reinterpret_cast<byte*>(iterator));

		*(reinterpret_cast<byte*>(iterator)) = newSequence[(iterator - start)];
	}

	// TODO Restore VirtualProtect
}

uintptr_t Memory::FindSequence(const wchar_t* moduleName, byte* sequence, unsigned int sequenceSize)
{
	HMODULE hModule = GetModuleHandle(moduleName); // L"ac_client.exe"
	MODULEINFO moduleInfo;

	GetModuleInformation(
		GetCurrentProcess(),
		hModule,
		&moduleInfo,
		sizeof(MODULEINFO)
	);

	const uintptr_t endAddress = (uintptr_t)hModule + moduleInfo.SizeOfImage;
	uintptr_t sequenceAddr = NULL;

	for (uintptr_t iterator = (uintptr_t)hModule; iterator < ((uintptr_t)hModule + moduleInfo.SizeOfImage); iterator++)
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


void Memory::PatchJump(uintptr_t start, void* hookFunction, const size_t sequenceSize, byte* oldSequence)
{
	if (sequenceSize < 5) return;

	// Allocate at least 5 bytes on stack; first byte = JMP, next 4 - target hook addy, rest: nop
	// pointer is cast to uintptr_t* to defer conversion to compiler - otherwise we'd need to manually populate each offset +1 +2 +3 +4 using bitshift
	// But then endinaness is important... Easier to leave it to compiler.
	byte* sequence = (byte*)alloca(sequenceSize * sizeof(byte));
	memset(sequence, 0x90, sequenceSize);

	// jmp (0xE9) - relative jump
	*sequence = 0xE9;

	// targetOffset = targetLocation - jumpLocation - jumpSize
	*(uintptr_t*)(sequence + 1) = (uintptr_t)hookFunction - start - 5;

	PatchSequence(start, sequence, sequenceSize, oldSequence);
}

// Returns Gateway address
void* Memory::TrampolineHook(uintptr_t start, void* hookFunction, const size_t sequenceSize) {
	// patch sequenceSize bytes and store them
	// add jmp to hookFunction instead
	byte* stolenBytes = (byte*)alloca(sequenceSize);

	PatchJump(start, hookFunction, sequenceSize, stolenBytes);
	
	// allocate memory
	// I don't know why Rake used VirtualAlloc - probably to allocate to separate page
	// byte* gatewayBytes = (byte*)VirtualAlloc(0, sequenceSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	byte* gatewayBytes = (byte*)malloc(sequenceSize);

	// Copy bytes to new memory
	memcpy_s(gatewayBytes, sequenceSize, stolenBytes, sequenceSize);

	// append jmp instruction
	*(gatewayBytes + sequenceSize) = 0xE9; // jmp

	// Jump has to be relative to gateway jumpoff point
	// current (gateway bytes, gatewayBytes + sequenceSize)
	// target (start + sequenceSize)
	// current + x = target
	// x = target - curent
	int relative_offset = (start + sequenceSize) - ((uintptr_t)gatewayBytes + sequenceSize) - 5;
	*((int*)(gatewayBytes + sequenceSize + 1)) = relative_offset;

	return gatewayBytes;
}