#include "GameController.h"

GameController::GameController()
{
    m_windowC = nullptr;
    m_renderer = nullptr;
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
}

void GameController::Run()
{
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