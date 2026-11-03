#include "GameController.h"

#include "WindowController.h"
#include "Renderer.h"
#include "Camera.h"
#include "ShaderPhong.h"
#include "Texture.h"
#include "Unit.h"

#include <algorithm>
#include <chrono>
#include <memory>
#include <random>
#include <vector>

namespace
{
    constexpr int CUBE_COUNT = 100;
    constexpr int COLUMNS = 10;

    const XMFLOAT3 LIGHT_START(0.0f, 3.0f, -3.0f);

    const wchar_t* DIFFUSE_TEXTURE =
        L"..\\Assets\\Textures\\MetalFrameWood.dds";

    const wchar_t* SPECULAR_TEXTURE =
        L"..\\Assets\\Textures\\MetalFrame.dds";

    class MultipleCubesScene : public IRenderable
    {
    public:
        MultipleCubesScene(
            const std::vector<std::unique_ptr<Unit>>& cubes,
            Unit& lightMarker,
            Shader* shader)
            : m_cubes(cubes),
            m_lightMarker(lightMarker)
        {
            SetShader(shader);
        }

        void Render(XMMATRIX viewProj) override
        {
            for (const auto& cube : m_cubes)
            {
                cube->Render(viewProj);
            }

            m_lightMarker.Render(viewProj);
        }

    private:
        const std::vector<std::unique_ptr<Unit>>& m_cubes;
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
    // All 100 cubes share this shader.
    ShaderPhong cubeShader;

    // The light marker uses a separate shader.
    ShaderPhong markerShader;

    cubeShader.Initialize();
    markerShader.Initialize();

    // Load one texture pair for all 100 cubes.
    Texture diffuseTexture;
    Texture specularTexture;

    diffuseTexture.Initialize(
        DIFFUSE_TEXTURE,
        &cubeShader);

    specularTexture.Initialize(
        SPECULAR_TEXTURE,
        &cubeShader);

    // Fill the marker shader's texture descriptors too.
    // Its pixel shader returns a solid color.
    Texture markerDiffuse;
    Texture markerSpecular;

    markerDiffuse.Initialize(
        DIFFUSE_TEXTURE,
        &markerShader);

    markerSpecular.Initialize(
        SPECULAR_TEXTURE,
        &markerShader);

    std::vector<std::unique_ptr<Unit>> cubes;
    cubes.reserve(CUBE_COUNT);

    std::mt19937 randomEngine(42);

    std::uniform_real_distribution<float> jitter(
        -0.12f, 0.12f);

    std::uniform_real_distribution<float> depth(
        0.0f, 4.0f);

    std::uniform_real_distribution<float> angle(
        -XM_PI, XM_PI);

    for (int i = 0; i < CUBE_COUNT; ++i)
    {
        auto cube = std::make_unique<Unit>();

        cube->Initialize(&cubeShader);

        const int column = i % COLUMNS;
        const int row = i / COLUMNS;

        const float x =
            (static_cast<float>(column) - 4.5f) * 1.35f
            + jitter(randomEngine);

        const float y =
            (static_cast<float>(row) - 4.5f) * 0.9f
            + jitter(randomEngine);

        const float z = depth(randomEngine);

        cube->SetPosition(XMFLOAT3(x, y, z));

        cube->SetScale(
            XMFLOAT3(0.65f, 0.65f, 0.65f));

        const float rotationX = angle(randomEngine);
        const float rotationY = angle(randomEngine);
        const float rotationZ = angle(randomEngine);

        cube->SetRotation(
            XMFLOAT3(rotationX, rotationY, rotationZ));

        cubes.push_back(std::move(cube));
    }

    Unit lightMarker;
    lightMarker.Initialize(&markerShader);

    lightMarker.SetPosition(LIGHT_START);
    lightMarker.SetScale(
        XMFLOAT3(0.3f, 0.3f, 0.3f));

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
        XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));

    cubeShader.SetLightColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    cubeShader.SetUseSpecularMap(true);

    // Use a more distant camera for the larger scene.
    const XMVECTOR cameraPosition =
        XMVectorSet(0.0f, 3.0f, -20.0f, 1.0f);

    const XMMATRIX view = XMMatrixLookAtLH(
        cameraPosition,
        XMVectorSet(0.0f, 0.0f, 2.0f, 1.0f),
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

    XMFLOAT4 cameraPositionData;
    XMStoreFloat4(&cameraPositionData, cameraPosition);

    cubeShader.SetCameraPosition(cameraPositionData);

    float* background = m_renderer->GetBGColor();
    background[0] = 0.025f;
    background[1] = 0.025f;
    background[2] = 0.04f;
    background[3] = 1.0f;

    MultipleCubesScene scene(
        cubes,
        lightMarker,
        &cubeShader);

    auto updateTitle = [&](bool mapEnabled)
        {
            SetWindowTextW(
                m_windowC->GetHWND(),
                mapEnabled
                ? L"Week 8 Task 2 | 100 cubes | Specular map ON | "
                L"Arrows: X/Y | Z/X: depth | R: reset | 1/2: map"
                : L"Week 8 Task 2 | 100 cubes | Specular map OFF | "
                L"Arrows: X/Y | Z/X: depth | R: reset | 1/2: map");
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

        const float movement = 4.0f * deltaTime;

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
            view * m_camera->GetProjection());
    }

    // Finish GPU work before destroying meshes and textures.
    m_renderer->WaitForPreviousFrame();
}