#include "GameController.h"

#include "WindowController.h"
#include "Renderer.h"
#include "Camera.h"
#include "ShaderPhong.h"
#include "Texture.h"
#include "Unit.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cwchar>
#include <memory>
#include <random>
#include <vector>

namespace
{
    constexpr int CUBE_COUNT = 100;
    constexpr int COLUMNS = 10;
    constexpr int LIGHT_COUNT = 5;

    const wchar_t* DIFFUSE_TEXTURE =
        L"..\\Assets\\Textures\\MetalFrameWood.dds";

    const wchar_t* SPECULAR_TEXTURE =
        L"..\\Assets\\Textures\\MetalFrame.dds";

    const wchar_t* LIGHT_NAMES[LIGHT_COUNT] =
    {
        L"Red",
        L"Green",
        L"Blue",
        L"Yellow",
        L"Magenta"
    };

    struct SceneLight
    {
        XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, -5.0f);
        XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

        float yawDegrees = 0.0f;
        float pitchDegrees = 0.0f;

        float coneDegrees = 35.0f;
        float falloff = 32.0f;

        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;

        float intensity = 1.5f;

        bool enabled = true;
    };

    ShaderPhong::Light MakeShaderLight(const SceneLight& source)
    {
        ShaderPhong::Light result = {};

        // Small ambient contribution per light.
        const float ambient = 0.015f * source.intensity;

        result.AmbientColor =
            XMFLOAT4(ambient, ambient, ambient, 1.0f);

        result.Position = source.position;

        result.Color = XMFLOAT4(
            source.color.x * source.intensity,
            source.color.y * source.intensity,
            source.color.z * source.intensity,
            1.0f);

        result.ConstLinQuad = XMFLOAT3(
            source.constant,
            source.linear,
            source.quadratic);

        const float yaw =
            XMConvertToRadians(source.yawDegrees);

        const float pitch =
            XMConvertToRadians(source.pitchDegrees);

        result.SpotDirection = XMFLOAT3(
            std::sin(yaw) * std::cos(pitch),
            std::sin(pitch),
            std::cos(yaw) * std::cos(pitch));

        result.SpotConeFalloff = XMFLOAT2(
            XMConvertToRadians(source.coneDegrees),
            source.falloff);

        return result;
    }

    class MultiLightScene : public IRenderable
    {
    public:
        MultiLightScene(
            const std::vector<std::unique_ptr<Unit>>& cubes,
            const std::vector<std::unique_ptr<Unit>>& markers,
            const std::array<SceneLight, LIGHT_COUNT>& lights,
            ShaderPhong& cubeShader,
            ShaderPhong& markerShader,
            const int& selectedLight)
            : m_cubes(cubes),
            m_markers(markers),
            m_lights(lights),
            m_markerShader(markerShader),
            m_selectedLight(selectedLight)
        {
            SetShader(&cubeShader);
        }

        void Render(XMMATRIX viewProj) override
        {
            for (const auto& cube : m_cubes)
            {
                cube->Render(viewProj);
            }

            for (int i = 0; i < LIGHT_COUNT; ++i)
            {
                const XMFLOAT4 markerColor =
                    m_lights[i].enabled
                    ? m_lights[i].color
                    : XMFLOAT4(0.12f, 0.12f, 0.12f, 1.0f);

                m_markerShader.SetDiffuseColor(markerColor);

                const float size =
                    i == m_selectedLight ? 0.38f : 0.24f;

                m_markers[i]->SetScale(
                    XMFLOAT3(size, size, size));

                // Each marker has its own constant-buffer slot.
                m_markers[i]->Render(viewProj);
            }
        }

    private:
        const std::vector<std::unique_ptr<Unit>>& m_cubes;
        const std::vector<std::unique_ptr<Unit>>& m_markers;
        const std::array<SceneLight, LIGHT_COUNT>& m_lights;

        ShaderPhong& m_markerShader;
        const int& m_selectedLight;
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
    ShaderPhong cubeShader;
    ShaderPhong markerShader;

    cubeShader.Initialize();
    markerShader.Initialize();

    Texture diffuseTexture;
    Texture specularTexture;

    diffuseTexture.Initialize(
        DIFFUSE_TEXTURE,
        &cubeShader);

    specularTexture.Initialize(
        SPECULAR_TEXTURE,
        &cubeShader);

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

    const std::array<XMFLOAT3, LIGHT_COUNT> startPositions =
    {
        XMFLOAT3(-5.0f,  2.0f, -5.0f),
        XMFLOAT3(-2.5f, -2.0f, -5.0f),
        XMFLOAT3(0.0f,  2.0f, -5.0f),
        XMFLOAT3(2.5f, -2.0f, -5.0f),
        XMFLOAT3(5.0f,  2.0f, -5.0f)
    };

    const std::array<XMFLOAT4, LIGHT_COUNT> startColors =
    {
        XMFLOAT4(1.0f, 0.12f, 0.08f, 1.0f),
        XMFLOAT4(0.12f, 1.0f, 0.12f, 1.0f),
        XMFLOAT4(0.12f, 0.30f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 0.80f, 0.08f, 1.0f),
        XMFLOAT4(0.90f, 0.10f, 1.0f, 1.0f)
    };

    std::array<SceneLight, LIGHT_COUNT> lights = {};

    for (int i = 0; i < LIGHT_COUNT; ++i)
    {
        lights[i].position = startPositions[i];
        lights[i].color = startColors[i];

        // Aim toward y = 0, z = 2 at this light's x coordinate.
        lights[i].pitchDegrees = XMConvertToDegrees(
            std::atan2(
                -lights[i].position.y,
                2.0f - lights[i].position.z));
    }

    const auto initialLights = lights;

    std::vector<std::unique_ptr<Unit>> markers;
    markers.reserve(LIGHT_COUNT);

    for (int i = 0; i < LIGHT_COUNT; ++i)
    {
        auto marker = std::make_unique<Unit>();

        marker->Initialize(&markerShader);
        marker->SetPosition(lights[i].position);

        markers.push_back(std::move(marker));
    }

    markerShader.SetIsLightCube(true);

    cubeShader.SetDiffuseColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    cubeShader.SetSpecularColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    cubeShader.SetSpecularPower(32.0f);
    cubeShader.SetSpecularStrength(1.0f);

    bool specularMapEnabled = true;

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

    int selectedLight = 0;
    UINT activeLightCount = LIGHT_COUNT;

    MultiLightScene scene(
        cubes,
        markers,
        lights,
        cubeShader,
        markerShader,
        selectedLight);

    auto updateTitle = [&]()
        {
            const auto& selected = lights[selectedLight];

            wchar_t title[256] = {};

            swprintf_s(
                title,
                _countof(title),
                L"Task 5 | Active %u/5 | Selected %d: %ls %ls | "
                L"Intensity %.2f | Half-angle %.1f | Falloff %.1f | Map %ls",
                activeLightCount,
                selectedLight + 1,
                LIGHT_NAMES[selectedLight],
                selected.enabled ? L"ON" : L"OFF",
                static_cast<double>(selected.intensity),
                static_cast<double>(selected.coneDegrees),
                static_cast<double>(selected.falloff),
                specularMapEnabled ? L"ON" : L"OFF");

            SetWindowTextW(m_windowC->GetHWND(), title);
        };

    updateTitle();

    using Clock = std::chrono::steady_clock;
    auto previousTime = Clock::now();

    bool previousSpace = false;
    bool previousM = false;

    float titleTimer = 0.0f;
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

        // Number keys select a light.
        for (int i = 0; i < LIGHT_COUNT; ++i)
        {
            if (pressed('1' + i))
                selectedLight = i;
        }

        // Enable all five lights.
        if (pressed('0'))
        {
            for (auto& light : lights)
                light.enabled = true;
        }

        // Keep only the selected light enabled.
        if (pressed('6'))
        {
            for (int i = 0; i < LIGHT_COUNT; ++i)
                lights[i].enabled = (i == selectedLight);
        }

        auto& selected = lights[selectedLight];

        // Toggle only on the initial key press.
        const bool spaceDown = pressed(VK_SPACE);

        if (spaceDown && !previousSpace)
            selected.enabled = !selected.enabled;

        previousSpace = spaceDown;

        const bool mDown = pressed('M');

        if (mDown && !previousM)
            specularMapEnabled = !specularMapEnabled;

        previousM = mDown;

        const float movement = 4.0f * deltaTime;

        selected.position.x +=
            (float(pressed(VK_RIGHT))
                - float(pressed(VK_LEFT))) * movement;

        selected.position.y +=
            (float(pressed(VK_UP))
                - float(pressed(VK_DOWN))) * movement;

        selected.position.z +=
            (float(pressed('X'))
                - float(pressed('Z'))) * movement;

        const float turnSpeed = 40.0f * deltaTime;

        selected.yawDegrees +=
            (float(pressed('L')) - float(pressed('J')))
            * turnSpeed;

        selected.pitchDegrees +=
            (float(pressed('I')) - float(pressed('K')))
            * turnSpeed;

        selected.yawDegrees =
            std::remainder(selected.yawDegrees, 360.0f);

        selected.pitchDegrees =
            (std::clamp)(selected.pitchDegrees, -85.0f, 85.0f);

        selected.coneDegrees +=
            (float(pressed('O')) - float(pressed('U')))
            * 20.0f * deltaTime;

        selected.coneDegrees =
            (std::clamp)(selected.coneDegrees, 5.0f, 80.0f);

        selected.falloff +=
            (float(pressed('T')) - float(pressed('G')))
            * 40.0f * deltaTime;

        selected.falloff =
            (std::clamp)(selected.falloff, 1.0f, 256.0f);

        // B increases intensity; V decreases it.
        selected.intensity +=
            (float(pressed('B')) - float(pressed('V')))
            * deltaTime;

        selected.intensity =
            (std::clamp)(selected.intensity, 0.0f, 4.0f);

        selected.constant +=
            (float(pressed('Q')) - float(pressed('A')))
            * 0.75f * deltaTime;

        selected.linear +=
            (float(pressed('W')) - float(pressed('S')))
            * 0.10f * deltaTime;

        selected.quadratic +=
            (float(pressed('E')) - float(pressed('D')))
            * 0.05f * deltaTime;

        selected.constant =
            (std::max)(selected.constant, 1.0f);

        selected.linear =
            (std::max)(selected.linear, 0.0f);

        selected.quadratic =
            (std::max)(selected.quadratic, 0.0f);

        if (pressed(VK_F1))
        {
            selected.constant = 1.0f;
            selected.linear = 0.0f;
            selected.quadratic = 0.0f;
        }

        if (pressed(VK_F2))
        {
            selected.constant = 1.0f;
            selected.linear = 0.09f;
            selected.quadratic = 0.032f;
        }

        if (pressed(VK_F3))
        {
            selected.constant = 1.0f;
            selected.linear = 0.35f;
            selected.quadratic = 0.12f;
        }

        if (pressed('R'))
        {
            selected = initialLights[selectedLight];
        }

        // Pack enabled lights into consecutive shader array slots.
        activeLightCount = 0;

        for (int i = 0; i < LIGHT_COUNT; ++i)
        {
            markers[i]->SetPosition(lights[i].position);

            if (lights[i].enabled)
            {
                cubeShader.SetLight(
                    activeLightCount,
                    MakeShaderLight(lights[i]));

                ++activeLightCount;
            }
        }

        cubeShader.SetLightCount(activeLightCount);
        cubeShader.SetUseSpecularMap(specularMapEnabled);

        titleTimer += deltaTime;

        if (titleTimer >= 0.15f)
        {
            updateTitle();
            titleTimer = 0.0f;
        }

        m_renderer->Render(
            &scene,
            view * m_camera->GetProjection());
    }

    m_renderer->WaitForPreviousFrame();
}