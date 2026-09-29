#pragma once

#include "StandardIncludes.h"
#include <shellscalingapi.h>

#pragma comment(lib, "shcore.lib")

LRESULT CALLBACK WndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam);

enum class WindowMode
{
    WINDOWED,
    WINDOWED_BORDERLESS,
    WINDOWED_BORDERLESS_FULLSCREEN,
    FULLSCREEN_EXCLUSIVE
};

class WindowController : public Singleton<WindowController>
{
public:
    WindowController();
    virtual ~WindowController() {}

    HWND GetHWND() const
    {
        return m_hwnd;
    }

    int GetWindowWidth() const
    {
        return m_windowWidth;
    }

    int GetWindowHeight() const
    {
        return m_windowHeight;
    }

    bool* GetKeyStates()
    {
        return m_keyStates;
    }

    void Create(
        HINSTANCE _hInstance,
        int _showWnd,
        WindowMode _mode);

private:
    void RegClass(HINSTANCE _hInstance);
    void GetPrimaryMonitorResolution();
    void CenterWindow();
    void DefaultSize();

    void Initialize(
        HINSTANCE _hInstance,
        int _showWnd,
        WindowMode _mode);

    LPCWSTR m_wClassName;
    LPCWSTR m_wTitle;

    WNDCLASSEX m_wClass;
    HWND m_hwnd;
    WindowMode m_windowMode;

    int m_windowPosX;
    int m_windowPosY;
    int m_windowWidth;
    int m_windowHeight;
    int m_monitorWidth;
    int m_monitorHeight;

    bool m_keyStates[256] = {};
};