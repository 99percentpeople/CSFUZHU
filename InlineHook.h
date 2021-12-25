#pragma once
#include "framework.h"

#define JMP 0xE9
constexpr int jmpCodeLength = 5;

class InlineHook {
protected:
	bool m_isHooked;

	BYTE m_original_byte[jmpCodeLength];
	BYTE m_self_byte[jmpCodeLength];
	
	LPVOID m_original_address;
	LPVOID m_self_address;
	
	DWORD motifyMemoryAttributes(LPVOID address, DWORD attributes = PAGE_EXECUTE_READWRITE);
public:
	InlineHook() = default;
	InlineHook(InlineHook&& rhs) = default;
	InlineHook(LPVOID original_address, LPVOID self_address);
	InlineHook& operator =(const InlineHook& rhs) = default;
	bool isHooked();

	void hook();
	void unhook();
};

