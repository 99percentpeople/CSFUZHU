#include "hookD3D9API.h"

inline void d3d_data::initialize_d3d9(HWND hWnd)
{
	//初始化D3D
	d3d_data::pdirect3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	checkError((BOOL)d3d_data::pdirect3d9, _T(" Direct3DCreate9失败"));

	D3DPRESENT_PARAMETERS present;
	ZeroMemory(&present, sizeof(D3DPRESENT_PARAMETERS));

	present.Windowed = TRUE;
	present.SwapEffect = D3DSWAPEFFECT_DISCARD;
	present.BackBufferFormat = D3DFMT_UNKNOWN;
	present.EnableAutoDepthStencil = TRUE;
	present.AutoDepthStencilFormat = D3DFMT_D16;
	present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	HRESULT resut = d3d_data::pdirect3d9->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&present,
		&d3d_data::pd3dDevice);
	checkError(SUCCEEDED(resut), _T("CreateDevice失败"));
}

inline void d3d_data::cleanup_device_d3d9()
{
	if (d3d_data::pLine) { d3d_data::pLine->Release(); }
	if(d3d_data::pFont) { d3d_data::pFont->Release(); }
	if(d3d_data::pd3dDevice) { d3d_data::pFont->Release(); }
	if (d3d_data::pdirect3d9) { d3d_data::pd3dDevice->Release(); }
}



extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK my_WNDPROC(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_F12)
		{
			g_data.show_menu = !g_data.show_menu;
		}
		else if (wParam == VK_F11)
		{
			g_data.g_cheat.init_game_data();

		}
		else if (wParam == VK_F9)
		{
			g_data.g_cheat.load_address(count);
			g_data.g_cheat.reflash_data();
		}
		else if (wParam == VK_F8)
		{
			g_data.reflash.store(true);
		}
		else if (wParam==VK_INSERT)
		{
			count++;
			printf("%d\n", count);
		}
		else if (wParam == VK_DELETE)
		{
			count--;
			printf("%d\n", count);
		}
		break;
	}
	return CallWindowProcW(g_data.g_original_proc, hWnd, msg, wParam, lParam);
}

//初始化ImGui
inline void initialize_imgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	static ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	// 加载中文字体
	CStringA font_path;
	font_path.GetBufferSetLength(128);
	GetWindowsDirectoryA(font_path.GetBuffer(), sizeof(font_path));
	font_path.Format("%s\\fonts\\msyh.ttc", font_path);

	if (::std::filesystem::exists(font_path.GetString()))
	{
		io.Fonts->AddFontFromFileTTF(font_path, 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
#ifdef _DEBUG
		CStringA debug_string;
		debug_string.Format("已加载字体文件: %s", font_path);
		OutputDebugStringA(debug_string);
	}
	else
	{
		CStringA debug_string;
		debug_string.Format("字体文件加载失败\n");
		OutputDebugStringA(debug_string);
#endif
	}
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	ImGui_ImplDX9_Init(d3d_data::pGameDirect3DDevice);
	ImGui_ImplWin32_Init(g_data.g_hGameWnd);
}

inline void createD3dDrawObject()
{
	D3DXCreateLine(d3d_data::pGameDirect3DDevice, &d3d_data::pLine);
	D3DXCreateFontW(d3d_data::pGameDirect3DDevice, 16, 0, FW_DONTCARE, D3DX_DEFAULT, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Vernada", &d3d_data::pFont);
}

HRESULT __stdcall Reset(IDirect3DDevice9* direct3ddevice9, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	d3d_data::g_Reset_hook->unhook();
	static BOOL first_call = true;
	if (first_call)
	{
#ifdef _DEBUG
		printf("Reset Hooked\n");
#endif

		first_call = false;
	}
	d3d_data::pLine->Release();
	d3d_data::pFont->Release();
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hresult = direct3ddevice9->Reset(pPresentationParameters);
	
	ImGui_ImplDX9_CreateDeviceObjects();
	createD3dDrawObject();

	d3d_data::pGameDirect3DDevice->GetViewport(&g_data.view_port);
	
	d3d_data::g_Reset_hook->hook();
	return hresult;
}

HRESULT __stdcall EndScene(IDirect3DDevice9* direct3ddevice9)
{
	d3d_data::g_EndScene_hook->unhook();
	static BOOL first_call = true;

	if (first_call)
	{
#ifdef _DEBUG
		printf("EndScene Hooked\n");
#endif

		d3d_data::pGameDirect3DDevice = direct3ddevice9;

		createD3dDrawObject();

		//初始化ImGui
		initialize_imgui();

		//劫持窗口消息
		g_data.g_original_proc = (WNDPROC)SetWindowLongW(g_data.g_hGameWnd, GWL_WNDPROC, (LONG)my_WNDPROC);
		
		d3d_data::pGameDirect3DDevice->GetViewport(&g_data.view_port);

		first_call = false;
	}

	if (g_data.reflash.load() == true)
	{
		g_data.g_cheat.reflash_data();
		
		g_data.reflash.store(false);
	}
	g_data.cheat_manager();
	g_data.draw_menu();
	HRESULT hresult = direct3ddevice9->EndScene();
	
	d3d_data::g_EndScene_hook->hook();
	return hresult;
}

HRESULT __stdcall DrawIndexedPrimitive(IDirect3DDevice9* direct3ddevice9, D3DPRIMITIVETYPE type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	d3d_data::g_DrawIndexedPrimitive_hook->unhook();

	//IDirect3DVertexBuffer9* vectex_buffer=nullptr;
	//UINT offset, stride;
	//direct3ddevice9->GetStreamSource(0, &vectex_buffer, &offset, &stride);

	HRESULT hresult = direct3ddevice9->DrawIndexedPrimitive(type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
	d3d_data::g_DrawIndexedPrimitive_hook->hook();
	return hresult;
}

#ifdef _DEBUG
inline void createConsole()
{
	AllocConsole();
	SetConsoleTitleW(_T("测试"));
	static FILE* stream = freopen("CON", "w", stdout);
}
#endif

inline void hookAPI()
{
	int* direct3d9_table = (int*)*(int*)d3d_data::pdirect3d9;
	int* direct3d9_device9_table = (int*)*(int*)d3d_data::pd3dDevice;
	//创建函数钩子
	d3d_data::g_Reset_hook =new InlineHook(reinterpret_cast<LPVOID>(direct3d9_device9_table[16]), Reset);
	d3d_data::g_EndScene_hook =new InlineHook(reinterpret_cast<LPVOID>(direct3d9_device9_table[42]), EndScene);
	d3d_data::g_DrawIndexedPrimitive_hook =new InlineHook(reinterpret_cast<LPVOID>(direct3d9_device9_table[82]), DrawIndexedPrimitive);
	//勾住函数
	d3d_data::g_Reset_hook->hook();
	d3d_data::g_EndScene_hook->hook();
}

