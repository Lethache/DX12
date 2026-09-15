#include "StandardIncludes.h"
#include "WindowController.h"
#include "Renderer.h"

#include <iostream>

int WINAPI main(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    WindowController* wc =
        &WindowController::GetInstance();

    wc->Create(
        hInstance,
        nShowCmd,
        WindowMode::WINDOWED);

    Renderer* r = &Renderer::GetInstance();
    r->ConfigurePipeline(false);

    const char* byte_addr =
        reinterpret_cast<const char*>(
            r->GetDevice().Get());

    std::cout <<
        static_cast<const void*>(byte_addr);

    // Main message loop
    MSG msg = {};

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}