
#include "GameController.h"

#include "WindowController.h"
#include "Renderer.h"
#include "ToolsController.h"
#include "Shader.h"
#include "Mesh.h"

// Constructor
GameController::GameController()
{
    m_windowC = nullptr;
    m_renderer = nullptr;
    m_toolsC = nullptr;
}

// Destructor
GameController::~GameController()
{
}

// Initialize game
void GameController::Initialize(
    HINSTANCE _hInstance,
    int _nShowCmd)
{
    // Initialize window controller
    m_windowC =
        &WindowController::GetInstance();

    m_windowC->Create(
        _hInstance,
        _nShowCmd,
        WindowMode::WINDOWED);

    // Initialize renderer
    m_renderer =
        &Renderer::GetInstance();

    m_renderer->ConfigurePipeline(
        false,
        m_windowC->GetHWND(),
        m_windowC->GetWindowWidth(),
        m_windowC->GetWindowHeight());

    // Initialize tools controller
    m_toolsC =
        &ToolsController::GetInstance();

    m_toolsC->LoadToolsLibrary();
}

// Main game loop
void GameController::Run()
{
    // Show tools window
    m_toolsC->ShowForm();

    // Create and initialize shader
    Shader shader;

    shader.Initialize(
        L"shaders.hlsl",
        m_renderer->GetDevice().Get());

    // Create and initialize mesh
    Mesh mesh;

    mesh.Initialize(
        m_renderer->GetDevice().Get(),
        &shader);

    // Windows message
    MSG message = {};

    // Main loop
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
            // Update background color
            m_renderer->GetBGColor()[0] =
                static_cast<float>(
                    m_toolsC->GetRedData());

            m_renderer->GetBGColor()[1] =
                static_cast<float>(
                    m_toolsC->GetGreenData());

            m_renderer->GetBGColor()[2] =
                static_cast<float>(
                    m_toolsC->GetBlueData());

            // Render mesh
            m_renderer->Render(&mesh);
        }
    }
}