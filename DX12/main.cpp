#include "StandardIncludes.h"
#include "WindowController.h"

int WINAPI main(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd
)
{
    WindowController* wc = &WindowController::GetInstance();

    wc->Create(
        hInstance,
        nShowCmd,
        WindowMode::WINDOWED
    );

    // Main message loop
    MSG msg = {};

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}