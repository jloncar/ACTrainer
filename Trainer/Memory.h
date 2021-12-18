#pragma once
#include <Windows.h>
#include <vector>
#include <Psapi.h>

namespace Memory {
	uintptr_t ResolvePtr(uintptr_t base, std::vector<int> offsets);
	uintptr_t FindSequence(const wchar_t* moduleName, byte* sequence, unsigned int sequenceSize);
	void PatchSequence(uintptr_t start, byte* newSequence, unsigned int sequenceSize, byte* oldSequence = nullptr);
	void PatchJump(uintptr_t start, void* hookFunction, size_t sequenceSize, byte* oldSequence = nullptr);
	void* TrampolineHook(uintptr_t start, void* hookFunction, const size_t sequenceSize);
}