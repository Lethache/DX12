#include "GameController.h"
#include "WindowController.h"
#include "Renderer.h"
#include "Camera.h"
#include "ToolsController.h"
#include "ShaderPhong.h"
#include "Unit.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>

class LightingScene : public IRenderable
{
public:
    LightingScene(Unit& cube, Unit& light)
        : m_cube(cube), m_light(light)
    {
        SetShader(cube.GetShader());
    }

    void Render(XMMATRIX viewProj) override
    {
        m_cube.Render(viewProj);
        m_light.Render(viewProj);
    }

private:
    Unit& m_cube;
    Unit& m_light;
};

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

    ShaderPhong cubeShader;
    cubeShader.Initialize();

    ShaderPhong lightShader;
    lightShader.Initialize();

    Unit cube;
    cube.Initialize(&cubeShader);
    cube.SetScale(XMFLOAT3(0.8f, 0.8f, 0.8f));

    Unit light;
    light.Initialize(&lightShader);
    light.SetScale(XMFLOAT3(0.15f, 0.15f, 0.15f));

    LightingScene scene(cube, light);

    XMFLOAT4 diffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    XMFLOAT4 ambientLight(0.15f, 0.15f, 0.15f, 1.0f);
    XMFLOAT4 lightColor(1.0f, 1.0f, 1.0f, 1.0f);

    XMFLOAT3 lightPosition(-0.8f, 0.8f, -1.0f);

    cubeShader.SetIsLightCube(false);

    cubeShader.SetSpecularColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    lightShader.SetIsLightCube(true);

    lightShader.SetDiffuseColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    int editMode = 4;
    bool rotating = true;
    bool previousSpace = false;

    using Clock = std::chrono::steady_clock;
    auto previousTime = Clock::now();

    MSG message = {};
    bool running = true;

    while (running)
    {
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
        {
            break;
        }

        auto currentTime = Clock::now();

        float deltaTime =
            std::chrono::duration<float>(
                currentTime - previousTime).count();

        previousTime = currentTime;
        deltaTime = std::min(deltaTime, 0.05f);

        // Poll the renderer window's controls directly.
        // This also supports the numeric keypad.
        bool active =
            GetForegroundWindow() == m_windowC->GetHWND();

        auto pressed = [active](int key) -> bool
            {
                return active &&
                    (GetAsyncKeyState(key) & 0x8000) != 0;
            };

        if (pressed(VK_ESCAPE))
        {
            running = false;
            continue;
        }

        if (pressed('1') || pressed(VK_NUMPAD1))
            editMode = 1;

        if (pressed('2') || pressed(VK_NUMPAD2))
            editMode = 2;

        if (pressed('3') || pressed(VK_NUMPAD3))
            editMode = 3;

        if (pressed('4') || pressed(VK_NUMPAD4))
            editMode = 4;

        bool space = pressed(VK_SPACE);

        if (space && !previousSpace)
        {
            rotating = !rotating;
        }

        previousSpace = space;

        float x =
            (static_cast<float>(pressed(VK_RIGHT)) -
                static_cast<float>(pressed(VK_LEFT))) * deltaTime;

        float y =
            (static_cast<float>(pressed(VK_UP)) -
                static_cast<float>(pressed(VK_DOWN))) * deltaTime;

        float z =
            (static_cast<float>(pressed(VK_PRIOR)) -
                static_cast<float>(pressed(VK_NEXT))) * deltaTime;

        if (editMode == 4)
        {
            lightPosition.x = std::clamp(
                lightPosition.x + x, -1.2f, 1.2f);

            lightPosition.y = std::clamp(
                lightPosition.y + y, -1.0f, 1.2f);

            lightPosition.z = std::clamp(
                lightPosition.z + z, -1.5f, 1.0f);
        }
        else
        {
            XMFLOAT4* color = &diffuseColor;

            if (editMode == 2)
                color = &ambientLight;

            if (editMode == 3)
                color = &lightColor;

            color->x = std::clamp(
                color->x + x, 0.0f, 1.0f);

            color->y = std::clamp(
                color->y + y, 0.0f, 1.0f);

            color->z = std::clamp(
                color->z + z, 0.0f, 1.0f);
        }

        if (rotating)
        {
            XMFLOAT3 rotation = cube.GetRotation();

            rotation.y = std::fmod(
                rotation.y + deltaTime * 0.6f,
                XM_2PI);

            cube.SetRotation(rotation);
        }

        light.SetPosition(lightPosition);

        // Recover camera position from the actual view matrix.
        XMMATRIX inverseView =
            XMMatrixInverse(nullptr, m_camera->GetView());

        XMFLOAT4 cameraPosition;

        XMStoreFloat4(
            &cameraPosition,
            inverseView.r[3]);

        cubeShader.SetCameraPosition(cameraPosition);
        cubeShader.SetLightPosition(lightPosition);

        cubeShader.SetDiffuseColor(diffuseColor);
        cubeShader.SetAmbientLight(ambientLight);
        cubeShader.SetLightColor(lightColor);

        // Read the sliders every frame.
        cubeShader.SetSpecularPower(
            m_toolsC->GetSpecularPower());

        cubeShader.SetSpecularStrength(
            m_toolsC->GetSpecularStrength());

        float* background = m_renderer->GetBGColor();

        background[0] =
            m_toolsC->GetRedData() ? 1.0f : 0.0f;

        background[1] =
            m_toolsC->GetGreenData() ? 1.0f : 0.0f;

        background[2] =
            m_toolsC->GetBlueData() ? 1.0f : 0.0f;

        background[3] = 1.0f;

        const wchar_t* mode = L"Object color";

        XMFLOAT3 values(
            diffuseColor.x,
            diffuseColor.y,
            diffuseColor.z);

        if (editMode == 2)
        {
            mode = L"Ambient";

            values = XMFLOAT3(
                ambientLight.x,
                ambientLight.y,
                ambientLight.z);
        }
        else if (editMode == 3)
        {
            mode = L"Light color";

            values = XMFLOAT3(
                lightColor.x,
                lightColor.y,
                lightColor.z);
        }
        else if (editMode == 4)
        {
            mode = L"Light position";
            values = lightPosition;
        }

        wchar_t title[256] = {};

        swprintf_s(
            title,
            _countof(title),
            L"Task 2 | %ls: %.2f %.2f %.2f | "
            L"Power %.0f | 1-4 select | Space pause",
            mode,
            values.x,
            values.y,
            values.z,
            m_toolsC->GetSpecularPower());

        SetWindowTextW(m_windowC->GetHWND(), title);

        XMMATRIX viewProj =
            m_camera->GetView() * m_camera->GetProjection();

        m_renderer->Render(&scene, viewProj);
    }

    m_renderer->WaitForPreviousFrame();
}