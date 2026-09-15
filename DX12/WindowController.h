#pragma once
#pragma comment(lib, "shcore.lib") // Link shcore.lib for SetProcessDpiAwareness

#include <shellscalingapi.h> // For SetProcessDpiAwareness

#include "StandardIncludes.h"

LRESULT CALLBACK WndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
);

enum class WindowMode
{
    WINDOWED,
    WINDOWED_BORDERLESS,
    WINDOWED_BORDERLESS_FULLSCREEN,
    FULLSCREEN_EXCLUSIVE // Not implemented yet, we need DirectX for that
};

class WindowController : public Singleton<WindowController>
{
public:
    // Constructor/Destructor
    WindowController();
    virtual ~WindowController() {}

    // Methods
    void Create(
        HINSTANCE _hInstance,
        int _showWnd,
        WindowMode _mode
    );

private:
    // Methods
    void RegClass(HINSTANCE _hInstance);
    void GetPrimaryMonitorResolution();
    void CenterWindow();
    void DefaultSize();

    void Initialize(
        HINSTANCE _hInstance,
        int _showWnd,
        WindowMode _mode
    );

    // Members
    LPCWSTR m_wClassName;
    LPCWSTR m_wTitle;
    WNDCLASSEX m_wClass;
    HWND m_hWnd;
    WindowMode m_windowMode;

    int m_windowPosX;
    int m_windowPosY;
    int m_windowWidth;
    int m_windowHeight;
    int m_monitorWidth;
    int m_monitorHeight;
};