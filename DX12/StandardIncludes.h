
#pragma once

// Windows configuration
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

// Standard C++ libraries
#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cstdlib>

// Microsoft COM smart pointers
#include <wrl/client.h>

// DirectX 12
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// DirectX 12 helper library
#include "include/d3dx12/d3dx12.h"

// Link DirectX libraries
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Namespaces
using Microsoft::WRL::ComPtr;

// Project utilities and interfaces
#include "Singleton.h"
#include "IRenderable.h"

// Debug assertion
#define M_ASSERT(_cond, _msg)                 \
    do                                       \
    {                                        \
        if (!(_cond))                        \
        {                                    \
            OutputDebugStringA(_msg);        \
            OutputDebugStringA("\n");        \
            std::abort();                    \
        }                                    \
    } while (0)