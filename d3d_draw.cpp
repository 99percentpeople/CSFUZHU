#include "hookD3D9API.h"

inline void d3d_data::draw_line(float x1, float y1, float x2, float y2, float Width, const D3DCOLOR color)
{
	D3DXVECTOR2 Vertex[2] = { {x1,y1},{x2,y2} };
	d3d_data::pLine->SetWidth(Width);
	d3d_data::pLine->Draw(Vertex, 2, color);
}

inline void d3d_data::draw_words(float X, float Y, const LPWSTR Str, const  D3DCOLOR Color)
{
	RECT Rect = { (LONG)X,(LONG)Y };
	d3d_data::pFont->DrawTextW(NULL, Str, -1, &Rect, DT_CALCRECT, Color);
	d3d_data::pFont->DrawTextW(NULL, Str, -1, &Rect, DT_LEFT, Color);
}

inline void d3d_data::draw_rect(float left, float top, float right, float bottom, float Width, const  D3DCOLOR color)
{
	D3DXVECTOR2 Vertex[5] = { {left,top},{right,top},{right,bottom},{left,bottom},{left,top} };
	d3d_data::pLine->Draw(Vertex, 5, color);
}

inline void d3d_data::draw_circle(float x, float y, float  r, float width, const D3DCOLOR color)
{
	static constexpr float accuracy = 2.f * (float)M_PI / 180.f;
	D3DXVECTOR2 Vertex[181];
	for (int i = 0; i < 180; i++)
	{
		Vertex[i] = { x + cos(i * accuracy) * r,y + sin(i * accuracy) * r };
	}
	Vertex[180] = Vertex[0];
	d3d_data::pLine->SetWidth(width);
	d3d_data::pLine->Draw(Vertex, 181, color);

}

inline void d3d_data::draw_bone(const PlayerData::boneMatrix bone_matrix[17], const  float viewMatrix[4][4], const  D3DVIEWPORT9& view_port, const  float& width, const D3DCOLOR& color)
{

	Vec2 screen[17];
	bool show[17] = { true };
	Vec3 pos[17];
	float head = 5;
	for (int i = 0; i < 17; i++)
	{
		pos[i] = Vec3(bone_matrix[i].m_data[0][3], bone_matrix[i].m_data[1][3], bone_matrix[i].m_data[2][3]);

		if (i == 0)
		{
			show[i] = WorldToScreen_4x4(pos[i], screen[i], view_port, viewMatrix, &head);
		}

		show[i] = WorldToScreen_4x4(pos[i], screen[i], view_port, viewMatrix);

	}
	if (show[0])
	{
		d3d_data::draw_circle(screen[0].x, screen[0].y, head, 1, color);
	}
	if (show[1] && show[0])
	{
		d3d_data::draw_line(screen[0].x, screen[0].y, screen[1].x, screen[1].y, width, color);
	}
	if (show[2] && show[1])
	{
		d3d_data::draw_line(screen[1].x, screen[1].y, screen[2].x, screen[2].y, width, color);
	}
	if (show[3] && show[1])
	{
		d3d_data::draw_line(screen[1].x, screen[1].y, screen[3].x, screen[3].y, width, color);
	}
	if (show[4] && show[3])
	{
		d3d_data::draw_line(screen[3].x, screen[3].y, screen[4].x, screen[4].y, width, color);
	}
	if (show[5] && show[4])
	{
		d3d_data::draw_line(screen[4].x, screen[4].y, screen[5].x, screen[5].y, width, color);
	}
	if (show[6] && show[1])
	{
		d3d_data::draw_line(screen[1].x, screen[1].y, screen[6].x, screen[6].y, width, color);
	}
	if (show[7] && show[6])
	{
		d3d_data::draw_line(screen[6].x, screen[6].y, screen[7].x, screen[7].y, width, color);
	}
	if (show[8] && show[7])
	{
		d3d_data::draw_line(screen[7].x, screen[7].y, screen[8].x, screen[8].y, width, color);
	}
	if (show[9] && show[2])
	{
		d3d_data::draw_line(screen[2].x, screen[2].y, screen[9].x, screen[9].y, width, color);
	}
	if (show[10] && show[9])
	{
		d3d_data::draw_line(screen[9].x, screen[9].y, screen[10].x, screen[10].y, width, color);
	}
	if (show[11] && show[10])
	{
		d3d_data::draw_line(screen[10].x, screen[10].y, screen[11].x, screen[11].y, width, color);
	}
	if (show[12] && show[11])
	{
		d3d_data::draw_line(screen[11].x, screen[11].y, screen[12].x, screen[12].y, width, color);
	}
	if (show[13] && show[2])
	{
		d3d_data::draw_line(screen[2].x, screen[2].y, screen[13].x, screen[13].y, width, color);
	}
	if (show[14] && show[13])
	{
		d3d_data::draw_line(screen[13].x, screen[13].y, screen[14].x, screen[14].y, width, color);
	}
	if (show[15] && show[14])
	{
		d3d_data::draw_line(screen[14].x, screen[14].y, screen[15].x, screen[15].y, width, color);
	}
	if (show[16] && show[15])
	{
		d3d_data::draw_line(screen[15].x, screen[15].y, screen[16].x, screen[16].y, width, color);
	}
}
void data::draw_menu()
{
	if (show_menu)
	{
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin(u8"CSGO游戏辅助", &show_menu);
		ImGui::Text(u8"[F12]		显示/关闭");
		ImGui::Checkbox(u8"敌人透视", &show_enemy);
		ImGui::SameLine();
		ImGui::Checkbox(u8"队友透视", &show_friend);
		ImGui::Separator();

		ImGui::End();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
}

void data::cheat_manager()
{
	static D3DCOLOR red = D3DCOLOR_XRGB(255, 0, 0);
	static D3DCOLOR green = D3DCOLOR_XRGB(0, 255, 0);
	static D3DCOLOR yellow = D3DCOLOR_XRGB(255, 255, 0);
	bool state = show_enemy || show_friend;

	if (state == false) { return; }

	for (auto& x : g_cheat.data_container)
	{
		if (x.iTeamNum == 2)
		{
			if (show_enemy)
			{
				if (g_cheat.self_team_num == 3)
				{
					if(x.bSpotted)
						d3d_data::draw_bone(x.m_pbone_matrix,g_cheat.m_pViewMatrix.m_ViewMatrix,view_port,1,red);
					else
						d3d_data::draw_bone(x.m_pbone_matrix, g_cheat.m_pViewMatrix.m_ViewMatrix, view_port, 1, yellow);
					//printf("draw t\n");
				}
			}
			if (show_friend)
			{
				if (g_cheat.self_team_num == 2)
				{
					d3d_data::draw_bone(x.m_pbone_matrix, g_cheat.m_pViewMatrix.m_ViewMatrix, view_port, 1, green);
					//printf("draw t\n");
				}

			}
		}
		else
		{
			if (show_enemy)
			{
				if (g_cheat.self_team_num == 2)
				{
					if (x.bSpotted)
						d3d_data::draw_bone(x.m_pbone_matrix, g_cheat.m_pViewMatrix.m_ViewMatrix, view_port, 1, red);
					else
						d3d_data::draw_bone(x.m_pbone_matrix, g_cheat.m_pViewMatrix.m_ViewMatrix, view_port, 1, yellow);
					//printf("draw ct\n");
				}
			}
			if (show_friend)
			{
				if (g_cheat.self_team_num == 3)
				{
					d3d_data::draw_bone(x.m_pbone_matrix, g_cheat.m_pViewMatrix.m_ViewMatrix, view_port, 1, green);
					//printf("draw ct\n");
				}
			}
		}
	}
}