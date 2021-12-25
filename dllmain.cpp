#include"framework.h"
#include"hookD3D9API.h"
#include "CheatApp.h"
HRESULT _stdcall _beginthread_proc(LPVOID lpParam)
{
#ifdef _DEBUG
	createConsole();
#endif
	try
	{
		CheatApp App(static_cast<HMODULE>(lpParam));
		if (App.InitializeD3D9Device())
		{
			return S_OK;
		}
		return E_FAIL;
	}
	catch (DllException &e)
	{
		MessageBox(nullptr, e.ToString(), L"HR Failed", MB_OK);
		return E_FAIL;
	}
}



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	static HANDLE thread;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		//创建线程
		g_data.dll_module = hModule;
		hide_self(hModule);

		thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(_beginthread_proc), hModule, 0, nullptr);
		WaitForSingleObject(thread, INFINITY);
		HRESULT res;
		GetExitCodeThread(thread, reinterpret_cast<LPDWORD>(&res));
		assert(SUCCEEDED(res));
		break;
	}
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}