#pragma once
#include "framework.h"
#include"InlineHook.h"
#include "help_func.hpp"
enum EntityListObjOffset
{
	iTeamNum_offset = 0xF4,
	iHealth_offset = 0x100,
	iPosition_offset = 0xA0,
	bSpotted_offset = 0x93D,
	boneMatrix_offset = 0x26A8,
	eyesAngles_offset = 0xB380,
};

class CheatApp
{
protected:
	HWND mhGameWnd;
	HMODULE mdllMoudle;
	WNDPROC mgameOriginalProc;
	HANDLE mprocessHandle;
	DWORD mprocessID;

	D3DVIEWPORT9 mviewPort;
	IDirect3D9* mpDirect3D9;
	IDirect3DDevice9* mpDirect3DDevice;
	IDirect3DDevice9* mpGameDirect3DDevice;

	ID3DXLine* mpLine;
	ID3DXFont* mpFont;

	InlineHook mResetHook;
	InlineHook mEndSceneHook;
	InlineHook mDrawIndexedPrimitiveHook;

	int* direct3d9_table;
	int* direct3d9_device9_table;
protected:
	static  HRESULT __stdcall MyReset(IDirect3DDevice9* direct3ddevice9, D3DPRESENT_PARAMETERS* pPresentationParameters){};
	static  HRESULT __stdcall MyEndScene(IDirect3DDevice9* direct3ddevice9){};

	static  HRESULT __stdcall DrawIndexedPrimitive(IDirect3DDevice9* direct3ddevice9, D3DPRIMITIVETYPE type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount){};
	
public:
	explicit CheatApp(HMODULE dllMoudle);
	bool InitializeD3D9Device();
	void CleanUpD3D9Device();
	bool CreateHook();
	bool HookD3DAPI();
	bool GetGameWindow();
};


