#include "GameController.h"
#include "WindowController.h"
#include "Renderer.h"
#include "Camera.h"
#include "ToolsController.h"
#include "Shader.h"
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
    HINSTANCE _hInstance, int _nShowCmd)
{
    m_windowC = &WindowController::GetInstance();

    m_windowC->Create(
        _hInstance, _nShowCmd, WindowMode::WINDOWED);

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

    Shader shader;
    shader.Initialize(L"shaders.hlsl");

    Unit unit;
    unit.Initialize(&shader);

    MSG message = {};

    while (message.message != WM_QUIT)
    {
        if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        else
        {
            float* bgColor = m_renderer->GetBGColor();

            bgColor[0] = m_toolsC->GetRedData() ? 1.0f : 0.0f;
            bgColor[1] = m_toolsC->GetGreenData() ? 1.0f : 0.0f;
            bgColor[2] = m_toolsC->GetBlueData() ? 1.0f : 0.0f;
            bgColor[3] = 1.0f;

            XMMATRIX viewProj =
                m_camera->GetView() * m_camera->GetProjection();

            m_renderer->Render(&unit, viewProj);
        }
    }

    m_renderer->WaitForPreviousFrame();
}