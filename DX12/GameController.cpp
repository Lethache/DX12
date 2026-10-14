#include "GameController.h"
#include "WindowController.h"
#include "Renderer.h"
#include "Camera.h"
#include "ToolsController.h"
#include "ShaderPhong.h"
#include "Unit.h"

GameController::GameController()
{
    m_windowC = nullptr;
    m_renderer = nullptr;
    m_camera = nullptr;
    m_toolsC = nullptr;
}

GameController::~GameController()
{
    delete m_camera;
    m_camera = nullptr;
}

void GameController::Initialize(
    HINSTANCE hInstance,
    int nShowCmd)
{
    m_windowC = &WindowController::GetInstance();

    m_windowC->Create(
        hInstance,
        nShowCmd,
        WindowMode::WINDOWED);

    m_renderer = &Renderer::GetInstance();

    m_renderer->ConfigurePipeline(
        false,
        m_windowC->GetHWND(),
        m_windowC->GetWindowWidth(),
        m_windowC->GetWindowHeight());

    m_camera = new Camera(
        m_windowC->GetWindowWidth(),
        m_windowC->GetWindowHeight());

    m_toolsC = &ToolsController::GetInstance();
    m_toolsC->LoadToolsLibrary();
}

void GameController::Run()
{
    m_toolsC->ShowForm();

    ShaderPhong shader;
    shader.Initialize();

    Unit unit;
    unit.Initialize(&shader);

    unit.SetRotation(
        XMFLOAT3(0.0f, XMConvertToRadians(45.0f), 0.0f));

    MSG message = {};
    bool running = true;

    while (running)
    {
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

        float* bgColor = m_renderer->GetBGColor();

        bgColor[0] = m_toolsC->GetRedData() ? 1.0f : 0.0f;
        bgColor[1] = m_toolsC->GetGreenData() ? 1.0f : 0.0f;
        bgColor[2] = m_toolsC->GetBlueData() ? 1.0f : 0.0f;
        bgColor[3] = 1.0f;

        auto pos = unit.GetPosition();
        auto rot = unit.GetRotation();
        auto scl = unit.GetScale();

        const bool* keys = m_windowC->GetKeyStates();

        if (keys[VK_RIGHT])
        {
            pos.x += 0.01f;
        }

        if (keys[VK_LEFT])
        {
            pos.x -= 0.01f;
        }

        if (keys[VK_UP])
        {
            pos.y += 0.01f;
        }

        if (keys[VK_DOWN])
        {
            pos.y -= 0.01f;
        }

        if (keys['Q'])
        {
            rot.y += 0.1f;
        }

        if (keys['W'])
        {
            rot.y -= 0.1f;
        }

        if (keys['A'])
        {
            scl.x -= 0.1f;
        }

        if (keys['S'])
        {
            scl.x += 0.1f;
        }

        unit.SetPosition(pos);
        unit.SetRotation(rot);
        unit.SetScale(scl);

        XMMATRIX viewProj =
            m_camera->GetView() * m_camera->GetProjection();

        m_renderer->Render(&unit, viewProj);
    }

    m_renderer->WaitForPreviousFrame();
}