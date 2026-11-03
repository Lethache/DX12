#include "GameController.h"

#include "WindowController.h"
#include "Renderer.h"
#include "Camera.h"
#include "ShaderPhong.h"
#include "Unit.h"

#include <algorithm>
#include <chrono>

namespace
{
    const XMFLOAT3 LIGHT_START(0.8f, 0.5f, -1.5f);

    const wchar_t* DIFFUSE_TEXTURE =
        L"..\\Assets\\Textures\\MetalFrameWood.dds";

    const wchar_t* SPECULAR_TEXTURE =
        L"..\\Assets\\Textures\\MetalFrame.dds";

    class LightingScene : public IRenderable
    {
    public:
        LightingScene(Unit& cube, Unit& lightMarker)
            : m_cube(cube),
            m_lightMarker(lightMarker)
        {
            SetShader(cube.GetShader());
        }

        void Render(XMMATRIX viewProj) override
        {
            m_cube.Render(viewProj);
            m_lightMarker.Render(viewProj);
        }

    private:
        Unit& m_cube;
        Unit& m_lightMarker;
    };
}

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
}

void GameController::Run()
{
    // Separate shaders give each object its own constant buffer.
    ShaderPhong cubeShader;
    ShaderPhong markerShader;

    cubeShader.Initialize();
    markerShader.Initialize();

    Unit cube;
    Unit lightMarker;

    cube.Initialize(
        &cubeShader,
        DIFFUSE_TEXTURE,
        SPECULAR_TEXTURE);

    // Mesh::Initialize expects two textures.
    // The marker shader returns a solid color instead of sampling them.
    lightMarker.Initialize(
        &markerShader,
        DIFFUSE_TEXTURE,
        SPECULAR_TEXTURE);

    cube.SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
    cube.SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
    cube.SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

    lightMarker.SetPosition(LIGHT_START);
    lightMarker.SetScale(XMFLOAT3(0.12f, 0.12f, 0.12f));

    markerShader.SetIsLightCube(true);
    markerShader.SetDiffuseColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    cubeShader.SetDiffuseColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    cubeShader.SetSpecularColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    cubeShader.SetSpecularPower(32.0f);
    cubeShader.SetSpecularStrength(1.0f);

    cubeShader.SetAmbientLight(
        XMFLOAT4(0.12f, 0.12f, 0.12f, 1.0f));

    cubeShader.SetLightColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    cubeShader.SetUseSpecularMap(true);

    XMFLOAT4 cameraPosition;

    XMStoreFloat4(
        &cameraPosition,
        XMMatrixInverse(nullptr, m_camera->GetView()).r[3]);

    cubeShader.SetCameraPosition(cameraPosition);

    float* background = m_renderer->GetBGColor();
    background[0] = 0.025f;
    background[1] = 0.025f;
    background[2] = 0.04f;
    background[3] = 1.0f;

    LightingScene scene(cube, lightMarker);

    auto updateTitle = [&](bool mapEnabled)
        {
            SetWindowTextW(
                m_windowC->GetHWND(),
                mapEnabled
                ? L"Week 8 Task 1 | Specular map ON | "
                L"Arrows: X/Y | Z/X: depth | R: reset | 1: OFF | 2: ON"
                : L"Week 8 Task 1 | Specular map OFF | "
                L"Arrows: X/Y | Z/X: depth | R: reset | 1: OFF | 2: ON");
        };

    bool specularMapEnabled = true;
    updateTitle(specularMapEnabled);

    using Clock = std::chrono::steady_clock;
    auto previousTime = Clock::now();

    bool running = true;

    while (running)
    {
        MSG message = {};

        while (PeekMessage(
            &message, nullptr, 0, 0, PM_REMOVE))
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
            break;

        const auto currentTime = Clock::now();

        const float deltaTime = (std::min)(
            std::chrono::duration<float>(
                currentTime - previousTime).count(),
            0.05f);

        previousTime = currentTime;

        const bool active =
            GetForegroundWindow() == m_windowC->GetHWND();

        auto pressed = [active](int key)
            {
                return active
                    && (GetAsyncKeyState(key) & 0x8000) != 0;
            };

        if (pressed(VK_ESCAPE))
            break;

        auto lightPosition = lightMarker.GetPosition();

        const float movement = 1.1f * deltaTime;

        lightPosition.x +=
            (float(pressed(VK_RIGHT))
                - float(pressed(VK_LEFT))) * movement;

        lightPosition.y +=
            (float(pressed(VK_UP))
                - float(pressed(VK_DOWN))) * movement;

        lightPosition.z +=
            (float(pressed('X'))
                - float(pressed('Z'))) * movement;

        if (pressed('R'))
            lightPosition = LIGHT_START;

        bool nextMapEnabled = specularMapEnabled;

        if (pressed('1'))
            nextMapEnabled = false;

        if (pressed('2'))
            nextMapEnabled = true;

        if (nextMapEnabled != specularMapEnabled)
        {
            specularMapEnabled = nextMapEnabled;

            cubeShader.SetUseSpecularMap(
                specularMapEnabled);

            updateTitle(specularMapEnabled);
        }

        lightMarker.SetPosition(lightPosition);
        cubeShader.SetLightPosition(lightPosition);

        m_renderer->Render(
            &scene,
            m_camera->GetView() * m_camera->GetProjection());
    }

    m_renderer->WaitForPreviousFrame();
}