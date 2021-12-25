#pragma once

#include"framework.h"
#include"help_func.hpp"
#include"InlineHook.h"
#include "GameStruct.h"

inline int count = 0;
struct PlayerData
{

	bool bSpotted=false;
	int iTeamNum=0;
	int iHealth=0;

	Vec3 m_pPosition;

	Vec2 m_peyesAngles;

	struct boneMatrix {
		float  m_data[3][4]; //3x4
	} m_pbone_matrix[17]; //max size 85
};

struct camera
{
	Vec3* cameraPosition;
	float* cameraFov;
};
enum EntityListObjOffset
{
	iTeamNum_offset = 0xF4,
	iHealth_offset = 0x100,
	iPosition_offset = 0xA0,
	bSpotted_offset = 0x93D,
	boneMatrix_offset = 0x26A8,
	eyesAngles_offset = 0xB380,
};

class Cheat
{
public:
	bool is_created;

	Client* client;
	Engine* engine;

	GlobalVars* globalVars;

	int* m_pEntityListAddress;
	int* m_pViewMatrixAddress;
	int* m_LocalPlayerAddress;
	
	ViewMatrix m_pViewMatrix;
	int* m_LocalPlayerEntityObj;
	

	D3DVIEWPORT9 view_port;


	std::vector<PlayerData>data_container;

	int self_team_num;

public:
	Cheat();
	void reflash_data();

	bool load_address(int count);

	bool init_game_data();

	void init_class();

	void release();
};


namespace d3d_data
{
	inline InlineHook* g_Reset_hook = nullptr;
	inline InlineHook* g_EndScene_hook = nullptr;
	inline InlineHook* g_DrawIndexedPrimitive_hook = nullptr;

	inline IDirect3D9* pdirect3d9 = nullptr;
	inline IDirect3DDevice9* pd3dDevice = nullptr;

	inline IDirect3DDevice9* pGameDirect3DDevice = nullptr;

	inline ID3DXLine* pLine = nullptr;
	inline ID3DXFont* pFont = nullptr;

	inline void initialize_d3d9(HWND hWnd);

	inline void cleanup_device_d3d9();

	inline void draw_line(float x1, float y1, float x2, float y2, float Width, const D3DCOLOR color);

	inline void draw_words(float X, float Y, const LPWSTR Str, const  D3DCOLOR Color);

	inline void draw_rect(float left, float top, float right, float bottom, float Width, const  D3DCOLOR color);

	inline void draw_circle(float x, float y, float  r, float width, const D3DCOLOR color);

	inline void draw_bone(const PlayerData::boneMatrix bone_matrix[17], const  float viewMatrix[4][4], const  D3DVIEWPORT9& view_port, const  float& width, const D3DCOLOR& color);

};
struct data
{
	Cheat g_cheat;
	std::atomic_bool is_inGame;
	std::atomic_bool reflash;
	HWND g_hGameWnd;
	HMODULE dll_module;
	DWORD pid;
	HANDLE g_proc;
	WNDPROC g_original_proc;
	D3DVIEWPORT9 view_port;
	
	bool show_menu;
	bool show_enemy;
	bool show_friend;
	bool free;
	data()
	{
		reflash= ATOMIC_VAR_INIT(false);
		is_inGame= ATOMIC_VAR_INIT(false);
		free = false;
		show_menu = true;
		show_enemy = false;
		show_friend = false;
		g_hGameWnd = NULL;
		g_original_proc = nullptr;
		
	}
	void draw_menu();

	void cheat_manager();
	
}inline g_data;



LRESULT CALLBACK my_WNDPROC(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//初始化ImGui
inline void initialize_imgui();

//创建 line,font
inline void createD3dDrawObject();

HRESULT __stdcall Reset(IDirect3DDevice9* direct3ddevice9, D3DPRESENT_PARAMETERS* pPresentationParameters);

HRESULT __stdcall EndScene(IDirect3DDevice9* direct3ddevice9);

HRESULT __stdcall DrawIndexedPrimitive(IDirect3DDevice9* direct3ddevice9, D3DPRIMITIVETYPE type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);

#ifdef _DEBUG
inline void createConsole();
#endif


inline void hookAPI();

HRESULT _stdcall _beginthread_proc(LPVOID lpParam);