#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <algorithm>

#include "Singleton.h"

#define M_ASSERT(_cond, _msg) \
    if (!(_cond)) { OutputDebugStringA(_msg); OutputDebugStringA(GetError()); std::abort(); }

static LPVOID lpErrorMessage;

static char* GetError()
{
    DWORD dwErrorCode = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        dwErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpErrorMessage,
        0,
        nullptr
    );

    return static_cast<char*>(lpErrorMessage);
};