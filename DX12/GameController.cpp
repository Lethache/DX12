#include "GameController.h"

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
    m_windowC =
        &WindowController::GetInstance();

    m_windowC->Create(
        _hInstance,
        _nShowCmd,
        WindowMode::WINDOWED);

    m_renderer =
        &Renderer::GetInstance();

    m_renderer->ConfigurePipeline(
        false,
        m_windowC->GetHWND(),
        m_windowC->GetWindowWidth(),
        m_windowC->GetWindowHeight());

    m_toolsC =
        &ToolsController::GetInstance();

    m_toolsC->LoadToolsLibrary();
}

void GameController::Run()
{
    m_toolsC->ShowForm();

    MSG msg = {};

    while (msg.message != WM_QUIT)
    {
        while (PeekMessage(
            &msg,
            nullptr,
            0,
            0,
            PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        m_renderer->GetBGColor()[0] =
            static_cast<float>(
                m_toolsC->GetRedData());

        m_renderer->GetBGColor()[1] =
            static_cast<float>(
                m_toolsC->GetGreenData());

        m_renderer->GetBGColor()[2] =
            static_cast<float>(
                m_toolsC->GetBlueData());

        m_renderer->GetBGColor()[3] = 1.0f;

        // Обновляем цвет окна
        m_renderer->Render();
    }
}