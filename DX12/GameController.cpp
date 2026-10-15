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

// Both objects are drawn during one Renderer::Render call.
class LightingScene : public IRenderable
{
public:
    LightingScene(Unit& texturedCube, Unit& lightCube)
        : m_texturedCube(texturedCube),
        m_lightCube(lightCube)
    {
        SetShader(texturedCube.GetShader());
    }

    void Render(XMMATRIX viewProj) override
    {
        m_texturedCube.Render(viewProj);
        m_lightCube.Render(viewProj);
    }

private:
    Unit& m_texturedCube;
    Unit& m_lightCube;
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

    // Each object has its own shader resources and constant buffer.
    ShaderPhong cubeShader;
    cubeShader.Initialize();

    ShaderPhong lightShader;
    lightShader.Initialize();

    Unit cube;
    cube.Initialize(&cubeShader);
    cube.SetScale(XMFLOAT3(0.8f, 0.8f, 0.8f));

    Unit lightCube;
    lightCube.Initialize(&lightShader);
    lightCube.SetScale(XMFLOAT3(0.15f, 0.15f, 0.15f));

    LightingScene scene(cube, lightCube);

    // Initial shader parameters are supplied by the application.
    XMFLOAT4 diffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    XMFLOAT4 ambientLight(0.2f, 0.2f, 0.2f, 1.0f);
    XMFLOAT4 lightColor(1.0f, 1.0f, 1.0f, 1.0f);

    XMFLOAT3 lightPosition(-1.0f, 0.8f, -0.8f);

    // White marker is rendered without texture or lighting.
    lightShader.SetIsLightCube(true);
    lightShader.SetDiffuseColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    cubeShader.SetIsLightCube(false);

    int editMode = 1;
    bool rotating = true;
    bool previousSpace = false;

    using Clock = std::chrono::steady_clock;
    auto previousTime = Clock::now();

    std::printf(
        "1: Object color\n"
        "2: Ambient light\n"
        "3: Light color\n"
        "4: Light position / direction\n"
        "Left/Right: R or X\n"
        "Down/Up: G or Y\n"
        "PageDown/PageUp: B or Z\n"
        "Space: Pause/resume rotation\n"
        "Escape: Exit\n");

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

        auto currentTime = Clock::now();

        float deltaTime =
            std::chrono::duration<float>(
                currentTime - previousTime).count();

        previousTime = currentTime;
        deltaTime = std::min(deltaTime, 0.05f);

        const bool* keys = m_windowC->GetKeyStates();

        if (keys['1']) editMode = 1;
        if (keys['2']) editMode = 2;
        if (keys['3']) editMode = 3;
        if (keys['4']) editMode = 4;

        bool spacePressed = keys[VK_SPACE];

        if (spacePressed && !previousSpace)
        {
            rotating = !rotating;
        }

        previousSpace = spacePressed;

        float changeX =
            (static_cast<float>(keys[VK_RIGHT]) -
                static_cast<float>(keys[VK_LEFT])) * deltaTime;

        float changeY =
            (static_cast<float>(keys[VK_UP]) -
                static_cast<float>(keys[VK_DOWN])) * deltaTime;

        float changeZ =
            (static_cast<float>(keys[VK_PRIOR]) -
                static_cast<float>(keys[VK_NEXT])) * deltaTime;

        if (editMode == 4)
        {
            XMFLOAT3 nextPosition = lightPosition;

            nextPosition.x += changeX;
            nextPosition.y += changeY;
            nextPosition.z += changeZ;

            nextPosition.x =
                std::clamp(nextPosition.x, -1.2f, 1.2f);

            nextPosition.y =
                std::clamp(nextPosition.y, -1.0f, 1.2f);

            nextPosition.z =
                std::clamp(nextPosition.z, -1.0f, 1.0f);

            float lengthSquared =
                nextPosition.x * nextPosition.x +
                nextPosition.y * nextPosition.y +
                nextPosition.z * nextPosition.z;

            // Keep a valid, nonzero direction.
            if (lengthSquared > 0.04f)
            {
                lightPosition = nextPosition;
            }
        }
        else
        {
            XMFLOAT4* selectedColor = &diffuseColor;

            if (editMode == 2)
            {
                selectedColor = &ambientLight;
            }
            else if (editMode == 3)
            {
                selectedColor = &lightColor;
            }

            selectedColor->x = std::clamp(
                selectedColor->x + changeX,
                0.0f,
                1.0f);

            selectedColor->y = std::clamp(
                selectedColor->y + changeY,
                0.0f,
                1.0f);

            selectedColor->z = std::clamp(
                selectedColor->z + changeZ,
                0.0f,
                1.0f);
        }

        if (rotating)
        {
            XMFLOAT3 rotation = cube.GetRotation();

            rotation.y = std::fmod(
                rotation.y + deltaTime * 0.7f,
                XM_2PI);

            cube.SetRotation(rotation);
        }

        lightCube.SetPosition(lightPosition);

        // Directional light points from the marker toward the origin.
        cubeShader.SetLightDirection(
            XMFLOAT3(
                -lightPosition.x,
                -lightPosition.y,
                -lightPosition.z));

        cubeShader.SetDiffuseColor(diffuseColor);
        cubeShader.SetAmbientLight(ambientLight);
        cubeShader.SetLightColor(lightColor);

        float* bgColor = m_renderer->GetBGColor();

        bgColor[0] = m_toolsC->GetRedData() ? 1.0f : 0.0f;
        bgColor[1] = m_toolsC->GetGreenData() ? 1.0f : 0.0f;
        bgColor[2] = m_toolsC->GetBlueData() ? 1.0f : 0.0f;
        bgColor[3] = 1.0f;

        const wchar_t* modeName = L"Object color";

        XMFLOAT3 displayedValues(
            diffuseColor.x,
            diffuseColor.y,
            diffuseColor.z);

        if (editMode == 2)
        {
            modeName = L"Ambient light";

            displayedValues = XMFLOAT3(
                ambientLight.x,
                ambientLight.y,
                ambientLight.z);
        }
        else if (editMode == 3)
        {
            modeName = L"Light color";

            displayedValues = XMFLOAT3(
                lightColor.x,
                lightColor.y,
                lightColor.z);
        }
        else if (editMode == 4)
        {
            modeName = L"Light position";

            displayedValues = lightPosition;
        }

        wchar_t title[256] = {};

        swprintf_s(
            title,
            _countof(title),
            L"Task 1 | %ls: %.2f, %.2f, %.2f | "
            L"1-4 select | Arrows + PgUp/PgDn | Space pause",
            modeName,
            displayedValues.x,
            displayedValues.y,
            displayedValues.z);

        SetWindowTextW(m_windowC->GetHWND(), title);

        XMMATRIX viewProj =
            m_camera->GetView() * m_camera->GetProjection();

        m_renderer->Render(&scene, viewProj);
    }

    m_renderer->WaitForPreviousFrame();
}