#include "WindowController.h"
#include "Renderer.h"

WindowController::WindowController()
{
    m_windowMode = WindowMode::WINDOWED;
    m_wClassName = L"DirectX12";
    m_wTitle = L"DirectX12 Renderer";
    m_wClass = {};
    m_hwnd = nullptr;

    m_windowPosX = 0;
    m_windowPosY = 0;
    m_windowWidth = 0;
    m_windowHeight = 0;
    m_monitorWidth = 0;
    m_monitorHeight = 0;
}

void WindowController::Create(
    HINSTANCE _hInstance,
    int _showWnd,
    WindowMode _mode)
{
    RegClass(_hInstance);
    Initialize(
        _hInstance,
        _showWnd,
        _mode);
}

void WindowController::RegClass(
    HINSTANCE _hInstance)
{
    m_wClass.cbSize = sizeof(WNDCLASSEX);

    m_wClass.style =
        WS_VISIBLE |
        WS_CLIPCHILDREN |
        WS_CLIPSIBLINGS;

    m_wClass.lpfnWndProc = WndProc;
    m_wClass.cbClsExtra = 0;
    m_wClass.cbWndExtra = 0;
    m_wClass.hInstance = _hInstance;

    m_wClass.hIcon =
        LoadIcon(nullptr, IDI_APPLICATION);

    m_wClass.hCursor =
        LoadCursor(nullptr, IDC_ARROW);

    m_wClass.hbrBackground =
        reinterpret_cast<HBRUSH>(
            COLOR_WINDOW + 2);

    m_wClass.lpszMenuName = nullptr;
    m_wClass.lpszClassName = m_wClassName;

    m_wClass.hIconSm =
        LoadIcon(nullptr, IDI_APPLICATION);

    M_ASSERT(
        RegisterClassEx(&m_wClass),
        "Failed to register the window class.");
}

void WindowController::GetPrimaryMonitorResolution()
{
    M_ASSERT(
        SetProcessDpiAwareness(
            PROCESS_SYSTEM_DPI_AWARE) == S_OK,
        "Failed to set process DPI awareness.");

    const POINT ptZero = { 0, 0 };

    HMONITOR primMon = MonitorFromPoint(
        ptZero,
        MONITOR_DEFAULTTOPRIMARY);

    M_ASSERT(
        primMon != nullptr,
        "Failed to retrieve primary monitor.");

    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(MONITORINFO);

    M_ASSERT(
        GetMonitorInfo(
            primMon,
            &monitorInfo) == TRUE,
        "Failed to get monitor info.");

    m_monitorWidth =
        monitorInfo.rcMonitor.right -
        monitorInfo.rcMonitor.left;

    m_monitorHeight =
        monitorInfo.rcMonitor.bottom -
        monitorInfo.rcMonitor.top;

    m_windowWidth = m_monitorWidth;
    m_windowHeight = m_monitorHeight;
}

void WindowController::CenterWindow()
{
    m_windowPosX =
        (m_monitorWidth / 2) -
        (m_windowWidth / 2);

    m_windowPosY =
        (m_monitorHeight / 2) -
        (m_windowHeight / 2);
}

void WindowController::DefaultSize()
{
    m_windowWidth = m_monitorWidth / 2;
    m_windowHeight = m_monitorHeight / 2;

    CenterWindow();
}

void WindowController::Initialize(
    HINSTANCE _hInstance,
    int _showWnd,
    WindowMode _mode)
{
    DWORD style = 0;

    m_windowMode = _mode;

    GetPrimaryMonitorResolution();

    switch (_mode)
    {
    case WindowMode::WINDOWED:
        style = WS_OVERLAPPEDWINDOW;
        DefaultSize();
        break;

    case WindowMode::WINDOWED_BORDERLESS:
        style = WS_POPUP;
        DefaultSize();
        break;

    case WindowMode::WINDOWED_BORDERLESS_FULLSCREEN:
        style = WS_POPUP;
        m_windowPosX = 0;
        m_windowPosY = 0;
        break;

    case WindowMode::FULLSCREEN_EXCLUSIVE:
        M_ASSERT(
            false,
            "Fullscreen exclusive mode is not implemented yet.");
        return;
    }

    m_hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        m_wClassName,
        m_wTitle,
        style,
        m_windowPosX,
        m_windowPosY,
        m_windowWidth,
        m_windowHeight,
        nullptr,
        nullptr,
        _hInstance,
        nullptr);

    M_ASSERT(
        m_hwnd != nullptr,
        "Failed to create window.");

    ShowWindow(
        m_hwnd,
        _showWnd);

    M_ASSERT(
        UpdateWindow(m_hwnd),
        "Failed to update window.");
}

LRESULT CALLBACK WndProc(
    HWND _hWnd,
    UINT _message,
    WPARAM _wParam,
    LPARAM _lParam)
{
    bool processed = true;

    switch (_message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_KEYUP:
        if (_wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        else
        {
            processed = false;
        }
        break;

    default:
        processed = false;
        break;
    }

    if (!processed)
    {
        return DefWindowProc(
            _hWnd,
            _message,
            _wParam,
            _lParam);
    }

    return 0;
}