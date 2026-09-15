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

    r->ConfigurePipeline(
        false,
        wc->GetHWND(),
        wc->GetWindowWidth(),
        wc->GetWindowHeight());

    // Some temporary debug output
    DXGI_SWAP_CHAIN_DESC scDesc;

    r->GetSwapChain().Get()->GetDesc(&scDesc);

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc =
        r->GetRTVHeap().Get()->GetDesc();

    std::cout
        << "Window Controller: Window - "
        << wc->GetHWND()
        << "\n"
        << "Swapchain: Buffer count - "
        << to_string(scDesc.BufferCount)
        << "; Window - "
        << scDesc.OutputWindow
        << "\n"
        << "RTV Heap: Number of descriptors - "
        << heapDesc.NumDescriptors;

    // Main message loop
    MSG msg = {};

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(
            &msg,
            NULL,
            0,
            0,
            PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}