#include "hookD3D9API.h"


Cheat::Cheat()
{
	m_pViewMatrix = {};
	m_pEntityListAddress = nullptr;
	m_pViewMatrixAddress = nullptr;
	m_pEntityListAddress = nullptr;
	is_created = false;
}

void Cheat::reflash_data()
{
	//printf("刷新数据\n");
	if (!data_container.empty())
	{
		data_container.clear();
	}
	SIZE_T read_size=0;
	if(m_LocalPlayerAddress != nullptr)
	{
		//读取自身
		ReadProcessMemory(g_data.g_proc, (LPVOID)(m_LocalPlayerAddress), &m_LocalPlayerEntityObj, sizeof(m_LocalPlayerEntityObj), &read_size);
		if (!read_size)
			return;
		//printf("读取自身\n");
	}
	if (m_pViewMatrixAddress != nullptr)
	{
		//读取自身矩阵
		ReadProcessMemory(g_data.g_proc, (LPVOID)m_pViewMatrixAddress, &m_pViewMatrix, sizeof(m_pViewMatrix), &read_size);
		if (!read_size)
			return;
		//printf("读取自身矩阵\n");
	}
	if (m_pEntityListAddress != nullptr )
	{
		EntityList el;
		ReadProcessMemory(g_data.g_proc, (LPVOID)(m_pEntityListAddress), &el, sizeof(EntityList), &read_size);
		

		 while (el.prev != nullptr){ReadProcessMemory(g_data.g_proc, (LPVOID)(el.prev), &el, sizeof(EntityList), &read_size);}
		
		for (; el.next != nullptr; ReadProcessMemory(g_data.g_proc, (LPVOID)(el.next), &el, sizeof(EntityList), &read_size))
		{
			PlayerData temp_data;
			//读取人物是否被发现
			ReadProcessMemory(g_data.g_proc, (LPVOID)(el.m_pEntityListObj+bSpotted_offset), &temp_data.bSpotted, sizeof(temp_data.bSpotted), &read_size);
			if(!read_size)
				return;
			//printf("读取人物是否被发现\n");
			//读取人物血量
			ReadProcessMemory(g_data.g_proc, (LPVOID)(el.m_pEntityListObj + iHealth_offset), &temp_data.iHealth, sizeof(temp_data.iHealth), &read_size);
			if (!read_size)
				return;
			if (!temp_data.iHealth)
				continue;
			//printf("读取人物血量\n");
			//读取人物阵营
			ReadProcessMemory(g_data.g_proc, (LPVOID)(el.m_pEntityListObj + iTeamNum_offset), &temp_data.iTeamNum, sizeof(temp_data.iTeamNum), &read_size);
			if (!read_size)
				return;
			if (!temp_data.iTeamNum)
				continue;
			//printf("读取人物阵营\n");
			//读取人物位置
			ReadProcessMemory(g_data.g_proc, (LPVOID)(el.m_pEntityListObj + iPosition_offset), &temp_data.m_pPosition, sizeof(temp_data.m_pPosition), &read_size);
			if (!read_size)
				return;
			//printf("读取人物位置\n");
			//读取人物朝向
			ReadProcessMemory(g_data.g_proc, (LPVOID)(el.m_pEntityListObj + eyesAngles_offset), &temp_data.m_peyesAngles, sizeof(temp_data.m_peyesAngles), &read_size);
			if (!read_size)
				return;
			//printf("读取人物朝向\n");
			//读取骨骼矩阵
			int* bone_matrix_address=nullptr;
			ReadProcessMemory(g_data.g_proc, (LPVOID)(el.m_pEntityListObj +  boneMatrix_offset), &bone_matrix_address, sizeof(bone_matrix_address), &read_size);
			if (!read_size)
				return;
			//printf("读取骨骼矩阵指针%p\n", bone_matrix_address);
			if(bone_matrix_address!=nullptr)
			{
				int bone_matrix_offset_t[17]= { 8,7,3,11,12,13,39,40,41,66,67,68,69,73,74,75,76 };
				int bone_matrix_offset_ct[17] = { 8,7,3,11,12,13,41,42,43,70,71,72,73,77,78,79,80 };
				if(temp_data.iTeamNum==2)
				{
					for (int i = 0; i < 17; i++)
					{
						ReadProcessMemory(g_data.g_proc, (LPVOID)(bone_matrix_address + 12 * (bone_matrix_offset_t[i]+count)), &temp_data.m_pbone_matrix[i], sizeof(PlayerData::boneMatrix), &read_size);
						if (!read_size)
							return;
					}
				}
				else if (temp_data.iTeamNum == 3)
				{
					for (int i = 0; i < 17; i++)
					{
						ReadProcessMemory(g_data.g_proc, (LPVOID)(bone_matrix_address + 12* (bone_matrix_offset_ct[i] + count)), &temp_data.m_pbone_matrix[i], sizeof(PlayerData::boneMatrix), &read_size);
						if (!read_size)
							return;
					}
				}
				
				
				if (!read_size)
					return;
			}
			//printf("读取骨骼矩阵\n");
			//判断是不是自己
			if(m_LocalPlayerEntityObj!=(int*)el.m_pEntityListObj)
			{
				data_container.push_back(temp_data);
				//printf("enemy added\n");
			}
			else
			{
				self_team_num = temp_data.iTeamNum;
				//printf("team=%d\n", self_team_num);
			}
		}
	}
}

bool Cheat::load_address(int count)
{
	/*
	B2 23 12 BE 25 49 12 3E
	E8 3F 12 BE 25 49 12 3E
	*/
	//通过特征码查找视角矩阵基址
	m_pViewMatrixAddress = reinterpret_cast<int*>(findPattern(_T("client"), "??\x12\xBE\x25\x49\x12\x3E", 8,0));
	if(m_pViewMatrixAddress==nullptr)
	{
		printf("Failed to find pattern m_pViewMatrixAddress\n");
		return false;
	}
	printf("[+] m_pViewMatrixAddress:%p\n", m_pViewMatrixAddress);
	
	m_pEntityListAddress = reinterpret_cast<int*>(findPattern(_T("client"),
		"\xA0\x20???????\x01\x00\x00\x00\x00\x00\x00\xEC\x31??", 20, 2+count));
	if (m_pEntityListAddress == nullptr)
	{
		printf("Failed to find pattern m_pEntityListAddress\n");
		return false;
	}
	printf("[+] m_pEntityListAddress:%p\n", m_pEntityListAddress);
	
	m_LocalPlayerAddress = reinterpret_cast<int*>(findPattern(_T("client"), "\xB3\x91??\x80\xA2??", 8,0)+0x0A);
	if (m_LocalPlayerAddress == nullptr)
	{
		printf("Failed to find pattern m_LocalPlayerAddress\n");
		return false;
	}
	printf("[+] m_LocalPlayerAddress:%p\n", m_LocalPlayerAddress);
	
	is_created = true;
	
	return true;
}


bool Cheat::init_game_data()
{
	if(load_address(0))
	{
		reflash_data();
		return true;
	}
	return false;
}

void Cheat::init_class()
{
	client = find<Client>(_T("client"), "VClient018");
	engine = find<Engine>(_T("engine"), "VEngineClient014");
	printf("client Address:%p\n", client);
	printf("engine Address:%p\n", engine);
}

void Cheat::release()
{
	if (!data_container.empty())
	{
		data_container.clear();
	}
}
