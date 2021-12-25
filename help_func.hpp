#pragma once

#include"framework.h"

struct Vec3 {
	float x, y, z;
	Vec3(float x = 0, float y = 0, float z = 0) :
		x(x), y(y), z(z) {}


};
struct Vec2 {
	float x, y;
	Vec2(float x = 0, float y = 0) :
		x(x), y(y) {}

};

inline void checkError(const BOOL state,const LPWSTR str = nullptr)
{
	if (state) return;

	WCHAR buffer[1024];
	wsprintf(buffer, _T("��������%s"),str);
	MessageBox(NULL, buffer, _T("����"), MB_OK | MB_ICONHAND);
	exit(-1);
}

inline DWORD GetProcessIDW(LPWSTR lpProcessName) {
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);// ���̿��վ��
	PROCESSENTRY32 process = { sizeof(PROCESSENTRY32) }; // ������Ϣ��ʼ��
	while (Process32Next(hProcessSnap, &process)) {
		if (wcscmp(process.szExeFile, lpProcessName) == 0) {
			DWORD processId = process.th32ProcessID;
			return processId;
		}
	}
	return NULL;
}

inline BOOL GetMODULEENTRY32(MODULEENTRY32 &me32,LPWSTR lpModuleName,DWORD process_ID)
{
	// ģ����վ��
	HANDLE  hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_ID); 	
	while (::Module32Next(hModuleSnap, &me32)) {//����ģ��
		if (_tcscmp(me32.szModule, lpModuleName) == 0) {
			return TRUE;
		}
	}
	return FALSE;
}

inline HANDLE GetProcessHnadle(DWORD process_ID)
{
	return OpenProcess(PROCESS_TERMINATE, FALSE, process_ID);
}



//�����麯��
template<typename T, typename ...Args>
constexpr auto callVirtualMethod(void* classBase, int index, Args... args) noexcept
{
	return ((*reinterpret_cast<T(__thiscall***)(void*, Args...)>(classBase))[index])(classBase, args...);
}

//��ȡָ����ָ��
template <typename T>
static auto find(const wchar_t* module, const char* name) noexcept
{
	if (HMODULE moduleHandle = GetModuleHandleW(module))
		if (const auto createInterface = reinterpret_cast<std::add_pointer_t<T * (const char* name, int* returnCode)>>(GetProcAddress(moduleHandle, "CreateInterface")))
			if (T* foundInterface = createInterface(name, nullptr))
				return foundInterface;
}

//��Ե�ַת��Ϊ���Ե�ַ
template <typename T>
static constexpr auto relativeToAbsolute(int* address) noexcept
{
	return reinterpret_cast<T>(reinterpret_cast<char*>(address + 1) + *address);
}

//�ڴ�ģʽ����
inline int* findPattern(const wchar_t* module, const char* pattern, size_t offset,int count=0)
{
	if (MODULEINFO moduleInfo;
		GetModuleInformation(GetCurrentProcess(), GetModuleHandleW(module), &moduleInfo, sizeof(moduleInfo)))
	{
		auto start = static_cast<const char*>(moduleInfo.lpBaseOfDll);
		const auto end = start + moduleInfo.SizeOfImage;

		auto first = start;
		auto second = pattern;
		while(true)
		{
			while (first < end && *second) {
				if (*first == *second || *second == '?') {
					++first;
					++second;
				}
				else {
					first = ++start;
					second = pattern;
				}
			}
			
			if(!*second)
			{
				if(count!=0)
				{
					count--;
					first = start + offset;
					second = pattern;
					continue;
				}
				return reinterpret_cast<int*>(const_cast<char*>(start) + offset);
			}
			return nullptr;
		}
	}
	return nullptr;
}

inline void hide_self(void* module)
{
	void* pPEB = nullptr;

	//��ȡPEBָ��
	_asm
	{
		push eax
		mov eax, fs: [0x30]
		mov pPEB, eax
		pop eax
	}

	//�����õ�����ȫ��ģ���˫������ͷָ��
	void* pLDR = *((void**)((unsigned char*)pPEB + 0x0c));
	void* pCurrent = *((void**)((unsigned char*)pLDR + 0x0c));
	void* pNext = pCurrent;

	//��������б�������ָ��ģ����ж�������
	do
	{
		void* pNextPoint = *((void**)((unsigned char*)pNext));
		void* pLastPoint = *((void**)((unsigned char*)pNext + 0x4));
		void* nBaseAddress = *((void**)((unsigned char*)pNext + 0x18));

		if (nBaseAddress == module)
		{
			*((void**)((unsigned char*)pLastPoint)) = pNextPoint;
			*((void**)((unsigned char*)pNextPoint + 0x4)) = pLastPoint;
			pCurrent = pNextPoint;
		}

		pNext = *((void**)pNext);
	} while (pCurrent != pNext);
}

inline bool WorldToScreen_4x4(const DirectX::XMVECTOR& world, DirectX::XMVECTOR&screen, const D3DVIEWPORT9& view_port, DirectX::XMMATRIX view_martix,float *size=nullptr) {
	Vec3 ndc;


	

	return true;

}

class DllException
{
public:
	DllException() = default;
	DllException(HRESULT hr, const CString & functionName, const CString& filename, int lineNumber);

	CString ToString()const
	{
		// Get the string description of the error code.
		_com_error err(ErrorCode);
		CString msg(err.ErrorMessage());

		CString ErrorInfo;
		ErrorInfo.Format(L"%s failed in %s; line %d; error: %s", FunctionName.GetString(),Filename.GetString(),LineNumber,msg.GetString());
		return ErrorInfo;
	}
protected:
	HRESULT ErrorCode = S_OK;
	CString FunctionName;
	CString Filename;
	int LineNumber = -1;
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    CString wfn = __FILE__;                                           \
    if(FAILED(hr__)) { throw DllException(hr__, L#x, wfn, __LINE__); }\
}
#endif
