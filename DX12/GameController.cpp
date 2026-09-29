#include "GameController.h"

#include "WindowController.h"
#include "Renderer.h"
#include "ToolsController.h"
#include "Shader.h"
#include "Camera.h"
#include "Unit.h"

#include <algorithm>
#include <chrono>

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

    Unit unit;

    unit.Initialize(
        m_renderer->GetDevice().Get(),
        &shader);

    // Match the projection to the render target dimensions.
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

    auto previousTime = std::chrono::steady_clock::now();

    MSG message = {};
    bool running = true;

    while (running)
    {
        // Process all pending window messages.
        while (PeekMessage(
            &message,
            nullptr,
            0,
            0,
            PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                running = false;
                break;
            }

            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        if (!running)
        {
            break;
        }

        // Calculate elapsed time for frame-independent movement.
        auto currentTime = std::chrono::steady_clock::now();

        float deltaTime = std::chrono::duration<float>(
            currentTime - previousTime).count();

        previousTime = currentTime;
        deltaTime = std::min(deltaTime, 0.05f);

        bool* keys = m_windowC->GetKeyStates();

        if (GetForegroundWindow() == m_windowC->GetHWND())
        {
            // Switch between the four camera setups.
            int selectedSetup = camera.GetSetup();

            if (keys['1'])
                selectedSetup = 1;
            else if (keys['2'])
                selectedSetup = 2;
            else if (keys['3'])
                selectedSetup = 3;
            else if (keys['4'])
                selectedSetup = 4;

            if (selectedSetup != camera.GetSetup())
            {
                camera.SetSetup(selectedSetup, aspectRatio);

                SetWindowTextW(
                    m_windowC->GetHWND(),
                    titles[selectedSetup - 1]);
            }

            auto position = unit.GetPosition();
            auto rotation = unit.GetRotation();
            auto scale = unit.GetScale();

            const float moveSpeed = 1.0f;
            const float rotationSpeed = 1.5f;
            const float scaleSpeed = 0.5f;

            // Move the object.
            if (keys[VK_RIGHT])
                position.x += moveSpeed * deltaTime;

            if (keys[VK_LEFT])
                position.x -= moveSpeed * deltaTime;

            if (keys[VK_UP])
                position.y += moveSpeed * deltaTime;

            if (keys[VK_DOWN])
                position.y -= moveSpeed * deltaTime;

            // Rotate around the Y axis.
            if (keys['Q'])
                rotation.y += rotationSpeed * deltaTime;

            if (keys['D'])
                rotation.y -= rotationSpeed * deltaTime;

            // Increase or decrease the width.
            if (keys['A'])
                scale.x += scaleSpeed * deltaTime;

            if (keys['S'])
                scale.x -= scaleSpeed * deltaTime;

            scale.x = std::max(scale.x, 0.1f);

            unit.SetPosition(position);
            unit.SetRotation(rotation);
            unit.SetScale(scale);
        }

        // Update the background color.
        m_renderer->GetBGColor()[0] =
            static_cast<float>(m_toolsC->GetRedData());

        m_renderer->GetBGColor()[1] =
            static_cast<float>(m_toolsC->GetGreenData());

        m_renderer->GetBGColor()[2] =
            static_cast<float>(m_toolsC->GetBlueData());

        shader.SetCameraMatrices(
            camera.GetView(),
            camera.GetProjection());

        m_renderer->Render(&unit);
    }
}