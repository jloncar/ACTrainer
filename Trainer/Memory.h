#pragma once
#include <Windows.h>
#include <vector>
#include <Psapi.h>

namespace Memory {
	uintptr_t ResolvePtr(uintptr_t base, std::vector<int> offsets);
	uintptr_t FindSequence(const wchar_t* moduleName, byte* sequence, unsigned int sequenceSize);
	void PatchSequence(uintptr_t start, byte* newSequence, unsigned int sequenceSize, byte* oldSequence = nullptr);
	void PatchJump(uintptr_t start, void* hookFunction, size_t sequenceSize, byte* oldSequence = nullptr);

	class TrampolineHook {
	private:
		uintptr_t m_Start;
		void* m_HookFunction;
		size_t m_StolenBytesSize;
		byte* m_StolenBytes;
		// pointer to gateway function, null if disabled
		byte* m_Gateway;

		bool m_Enabled;

	public:

		TrampolineHook(uintptr_t start, void* hookFunction, const size_t sequenceSize);
		~TrampolineHook();

		void Enable();
		void Disable();
		void* Gateway();
		bool IsEnabled();
	};
}