#pragma once
#pragma comment(lib, "dxguid.lib")
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <algorithm>
#include <wrl.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <cassert>
#include <climits>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <include/d3dx12/d3dx12.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;

#ifndef M_ASSERT
#define M_ASSERT(condition, message)                       \
    do                                                    \
    {                                                     \
        if (!(condition))                                 \
        {                                                 \
            MessageBoxA(nullptr, message, "DX12 Error",    \
                MB_OK | MB_ICONERROR);                    \
            ExitProcess(1);                               \
        }                                                 \
    } while (false)
#endif

#include "Singleton.h"

class WindowController;
class Renderer;
class Camera;
class ToolsController;
class CommandController;
class Shader;
class Texture;
class Mesh;
class Unit;
class IRenderable;