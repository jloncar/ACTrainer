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
	HMODULE hModule = GetModuleHandle(moduleName);
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
	byte* sequence = (byte*)alloca(sequenceSize * sizeof(byte));
	memset(sequence, 0x90, sequenceSize);

	// jmp (0xE9) - relative jump
	*sequence = 0xE9;

	// targetOffset = targetLocation - jumpLocation - jumpSize
	// pointer is cast to uintptr_t* to defer conversion to compiler - otherwise we'd need to manually populate each offset +1 +2 +3 +4 using bitshift
	// But then endinaness is important... Easier to leave it to compiler.
	*(uintptr_t*)(sequence + 1) = (uintptr_t)hookFunction - start - 5;

	PatchSequence(start, sequence, sequenceSize, oldSequence);
}


Memory::TrampolineHook::TrampolineHook(uintptr_t start, void* hookFunction, const size_t sequenceSize) :
	m_Start(start), m_HookFunction(hookFunction), m_StolenBytesSize(sequenceSize), m_StolenBytes(nullptr), m_Gateway(nullptr), m_Enabled(false) {}

void Memory::TrampolineHook::Enable()
{
	m_Enabled = true;
	// allocate memory
	// I don't know why Rake used VirtualAlloc - probably to allocate to separate page
	// byte* gatewayBytes = (byte*)VirtualAlloc(0, sequenceSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	m_Gateway = new byte[m_StolenBytesSize + 1 + sizeof(int)];
	m_StolenBytes = new byte[m_StolenBytesSize];

	// patch sequenceSize bytes and store them
	// add jmp to hookFunction instead
	PatchJump(m_Start, m_HookFunction, m_StolenBytesSize, m_StolenBytes);

	// Copy bytes to new memory
	memcpy_s(m_Gateway, m_StolenBytesSize, m_StolenBytes, m_StolenBytesSize);

	// append jmp instruction
	*(m_Gateway + m_StolenBytesSize) = 0xE9; // jmp

	// Jump has to be relative to gateway jumpoff point
	// current (gateway bytes, gatewayBytes + sequenceSize)
	// target (start + sequenceSize)
	// current + x = target
	// x = target - curent
	int relative_offset = (m_Start + m_StolenBytesSize) - ((uintptr_t)m_Gateway + m_StolenBytesSize) - 5;
	*((int*)(m_Gateway + m_StolenBytesSize + 1)) = relative_offset;

}

void Memory::TrampolineHook::Disable()
{
	m_Enabled = false;
	// Restore stolen bytes to original function
	PatchSequence(m_Start, m_StolenBytes, m_StolenBytesSize);

	// Free
	delete[] m_Gateway;
	delete[] m_StolenBytes;
}

void* Memory::TrampolineHook::Gateway()
{
	return (m_Enabled) ? m_Gateway : (void*)m_Start;
}

bool Memory::TrampolineHook::IsEnabled()
{
	return m_Enabled;
}

Memory::TrampolineHook::~TrampolineHook() {
	if (m_Enabled)
	{
		Disable();
	}
}