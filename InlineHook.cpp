
#include "InlineHook.h"

InlineHook::InlineHook(LPVOID original_address, LPVOID self_address):
	m_original_address(original_address), m_self_address(self_address)
{
	//������תָ��
	m_self_byte[0] = JMP;//jmpָ��
	int offset= (int)self_address - ((int)original_address + jmpCodeLength);//����ƫ��;
	memcpy(&m_self_byte[1],&offset, jmpCodeLength - 1);

	//����ԭʼ������ַ
	DWORD old_attributes = motifyMemoryAttributes(original_address);
	memcpy(m_original_byte, original_address, jmpCodeLength);
	motifyMemoryAttributes(original_address, old_attributes);

	m_isHooked = FALSE;
}

DWORD InlineHook::motifyMemoryAttributes(LPVOID address, DWORD attributes)
{
	DWORD old_attributes;
	if (!VirtualProtect(address, jmpCodeLength, attributes, &old_attributes)) 
	{
		printf("motifyMemoryAttributesʧ��\n");
		
	}
	return old_attributes;
}

bool InlineHook::isHooked()
{
	return m_isHooked;
}

void InlineHook::hook()
{
	DWORD old_attributes = motifyMemoryAttributes(m_original_address);
	memcpy(m_original_address, m_self_byte, jmpCodeLength);
	motifyMemoryAttributes(m_original_address, old_attributes);

	m_isHooked = TRUE;
}

void InlineHook::unhook()
{
	DWORD old_attributes = motifyMemoryAttributes(m_original_address);
	memcpy(m_original_address, m_original_byte, jmpCodeLength);
	motifyMemoryAttributes(m_original_address, old_attributes);

	m_isHooked = FALSE;
}
