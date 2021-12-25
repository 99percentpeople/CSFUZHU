#include "CheatApp.h"

CheatApp::CheatApp(HMODULE dllMoudle)
{
	mdllMoudle = dllMoudle;
}

bool CheatApp::InitializeD3D9Device()
{
	mpDirect3D9= Direct3DCreate9(D3D_SDK_VERSION);
	assert(mpDirect3D9);
	D3DPRESENT_PARAMETERS present;
	ZeroMemory(&present, sizeof(D3DPRESENT_PARAMETERS));
	
	present.Windowed = TRUE;
	present.SwapEffect = D3DSWAPEFFECT_DISCARD;
	present.BackBufferFormat = D3DFMT_UNKNOWN;
	present.EnableAutoDepthStencil = TRUE;
	present.AutoDepthStencilFormat = D3DFMT_D16;
	present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	ThrowIfFailed(mpDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mhGameWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &present, &mpDirect3DDevice))
	
	return true;
}

bool CheatApp::CreateHook()
{
	direct3d9_table = (int*)*(int*)mpDirect3D9;
	direct3d9_device9_table = (int*)*(int*)mpDirect3DDevice;
	InlineHook(reinterpret_cast<LPVOID>(direct3d9_device9_table[16]),MyReset);
}

bool CheatApp::GetGameWindow()
{
	
}
