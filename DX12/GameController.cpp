#include "GameController.h"

#include "WindowController.h"
#include "Renderer.h"
#include "ToolsController.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"

GameController::GameController()
{
    m_windowC = nullptr;
    m_renderer = nullptr;
    m_toolsC = nullptr;
}

GameController::~GameController()
{
}

void GameController::Initialize(
    HINSTANCE _hInstance,
    int _nShowCmd)
{
    m_windowC = &WindowController::GetInstance();

    m_windowC->Create(
        _hInstance,
        _nShowCmd,
        WindowMode::WINDOWED);

    m_renderer = &Renderer::GetInstance();

    m_renderer->ConfigurePipeline(
        false,
        m_windowC->GetHWND(),
        m_windowC->GetWindowWidth(),
        m_windowC->GetWindowHeight());

    m_toolsC = &ToolsController::GetInstance();
    m_toolsC->LoadToolsLibrary();
}

void GameController::Run()
{
    m_toolsC->ShowForm();

    Shader shader;

    shader.Initialize(
        L"shaders.hlsl",
        m_renderer->GetDevice().Get());

    Mesh mesh;

    mesh.Initialize(
        m_renderer->GetDevice().Get(),
        &shader);

    // Match the projection aspect ratio to the render target.
    DXGI_SWAP_CHAIN_DESC1 swapChainDescription = {};

    HRESULT result = m_renderer->GetSwapChain()->GetDesc1(
        &swapChainDescription);

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to read swap chain dimensions.");

    float aspectRatio =
        static_cast<float>(swapChainDescription.Width)
        / static_cast<float>(swapChainDescription.Height);

    Camera camera;
    camera.SetSetup(1, aspectRatio);

    const wchar_t* titles[] =
    {
        L"Setup 1: View 1 / FOV 45 | Keys 1-4",
        L"Setup 2: View 1 / FOV 75 | Keys 1-4",
        L"Setup 3: View 2 / FOV 45 | Keys 1-4",
        L"Setup 4: View 2 / FOV 75 | Keys 1-4"
    };

    SetWindowTextW(
        m_windowC->GetHWND(),
        titles[0]);

    MSG message = {};

    while (message.message != WM_QUIT)
    {
        if (PeekMessage(
            &message,
            nullptr,
            0,
            0,
            PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        else
        {
            // Process number keys only when the game has focus.
            if (GetForegroundWindow() == m_windowC->GetHWND())
            {
                int selectedSetup = camera.GetSetup();

                if (GetAsyncKeyState('1') & 0x8000)
                    selectedSetup = 1;
                else if (GetAsyncKeyState('2') & 0x8000)
                    selectedSetup = 2;
                else if (GetAsyncKeyState('3') & 0x8000)
                    selectedSetup = 3;
                else if (GetAsyncKeyState('4') & 0x8000)
                    selectedSetup = 4;

                if (selectedSetup != camera.GetSetup())
                {
                    camera.SetSetup(
                        selectedSetup,
                        aspectRatio);

                    SetWindowTextW(
                        m_windowC->GetHWND(),
                        titles[selectedSetup - 1]);
                }
            }

            // Update the background color from the tools window.
            m_renderer->GetBGColor()[0] =
                static_cast<float>(m_toolsC->GetRedData());

            m_renderer->GetBGColor()[1] =
                static_cast<float>(m_toolsC->GetGreenData());

            m_renderer->GetBGColor()[2] =
                static_cast<float>(m_toolsC->GetBlueData());

            // Send the active camera matrices to the shader.
            shader.SetCameraMatrices(
                camera.GetView(),
                camera.GetProjection());

            m_renderer->Render(&mesh);
        }
    }
}