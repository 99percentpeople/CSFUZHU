#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
#define _USE_MATH_DEFINES

// Windows 头文件
#include <windows.h>

// d3d
#pragma warning(push)
#pragma warning(disable:26495 26812)
#include<d3dx9.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#pragma warning(pop)
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

// imgui
#include"ImGui\imgui.h"
#include"ImGui\imgui_impl_dx9.h"
#include"ImGui\imgui_impl_win32.h"

//std
#include<memory>
#include<iostream>
#include <functional>
#include <filesystem>
#include<vector>
#include <cmath>
#include <string>
#include <atomic> 

//Windows
#include <TlHelp32.h>
#include<process.h>
#include <atlstr.h>
#include <atlsimpstr.h>
#include <Psapi.h>
#include <comdef.h>
#include <wrl.h>