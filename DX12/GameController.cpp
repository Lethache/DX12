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

namespace
{
    enum class Mode
    {
        MoveLight = 0,
        ColorByPosition = 1,
        MoveToCube = 2
    };

    const XMFLOAT3 LIGHT_START(1.0f, -0.6f, -0.8f);

    // Centered cube with its original texture colors.
    const XMFLOAT3 TARGET_START(0.0f, 0.0f, 0.0f);

    // Separate starting position for the chase mode.
    const XMFLOAT3 CHASE_START(0.0f, 0.2f, 0.7f);

    const XMFLOAT3 WOOD_CENTER(0.0f, 0.0f, 0.0f);
    const XMFLOAT3 WOOD_CHASE_POSITION(0.45f, -0.15f, 0.0f);

    const XMFLOAT3 WOOD_SCALE(0.8f, 0.8f, 0.8f);
    const XMFLOAT3 COLOR_SCALE(0.8f, 0.8f, 0.8f);
    const XMFLOAT3 CHASE_SCALE(0.8f, 0.8f, 0.8f);
    const XMFLOAT3 LIGHT_SCALE(0.15f, 0.15f, 0.15f);

    bool CubesTouch(const Unit& light, const Unit& target)
    {
        const auto lightPosition = light.GetPosition();
        const auto targetPosition = target.GetPosition();

        const auto lightScale = light.GetScale();
        const auto targetScale = target.GetScale();

        const auto rotation = target.GetRotation();

        const XMMATRIX targetRotation =
            XMMatrixRotationRollPitchYaw(
                rotation.x,
                rotation.y,
                rotation.z);

        const XMVECTOR lightAxes[3] =
        {
            XMVectorSet(1, 0, 0, 0),
            XMVectorSet(0, 1, 0, 0),
            XMVectorSet(0, 0, 1, 0)
        };

        const XMVECTOR targetAxes[3] =
        {
            targetRotation.r[0],
            targetRotation.r[1],
            targetRotation.r[2]
        };

        const float lightHalfSize[3] =
        {
            lightScale.x * 0.5f,
            lightScale.y * 0.5f,
            lightScale.z * 0.5f
        };

        const float targetHalfSize[3] =
        {
            targetScale.x * 0.5f,
            targetScale.y * 0.5f,
            targetScale.z * 0.5f
        };

        const XMVECTOR delta = XMVectorSubtract(
            XMLoadFloat3(&targetPosition),
            XMLoadFloat3(&lightPosition));

        auto separated = [&](XMVECTOR axis)
            {
                const float axisLengthSquared =
                    XMVectorGetX(XMVector3LengthSq(axis));

                if (axisLengthSquared < 1e-8f)
                    return false;

                float radius = 0.0f;

                for (int i = 0; i < 3; ++i)
                {
                    radius += lightHalfSize[i] * std::fabs(
                        XMVectorGetX(
                            XMVector3Dot(lightAxes[i], axis)));

                    radius += targetHalfSize[i] * std::fabs(
                        XMVectorGetX(
                            XMVector3Dot(targetAxes[i], axis)));
                }

                const float distance = std::fabs(
                    XMVectorGetX(XMVector3Dot(delta, axis)));

                return distance > radius + 1e-5f;
            };

        for (int i = 0; i < 3; ++i)
        {
            if (separated(lightAxes[i]) ||
                separated(targetAxes[i]))
            {
                return false;
            }

            for (int j = 0; j < 3; ++j)
            {
                if (separated(
                    XMVector3Cross(
                        lightAxes[i],
                        targetAxes[j])))
                {
                    return false;
                }
            }
        }

        return true;
    }

    class MidtermScene : public IRenderable
    {
    public:
        MidtermScene(Unit& wood, Unit& target, Unit& light)
            : m_wood(wood),
            m_target(target),
            m_light(light)
        {
            SetShader(wood.GetShader());
        }

        Mode mode = Mode::MoveLight;

        void Render(XMMATRIX viewProj) override
        {
            if (mode != Mode::ColorByPosition)
                m_wood.Render(viewProj);

            if (mode != Mode::MoveLight)
                m_target.Render(viewProj);

            if (mode != Mode::ColorByPosition)
                m_light.Render(viewProj);
        }

    private:
        Unit& m_wood;
        Unit& m_target;
        Unit& m_light;
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

    m_toolsC = &ToolsController::GetInstance();
    m_toolsC->LoadToolsLibrary();
}

void GameController::Run()
{
    m_toolsC->ShowForm();

    ShaderPhong woodShader;
    ShaderPhong targetShader;
    ShaderPhong lightShader;

    woodShader.Initialize();
    targetShader.Initialize();
    lightShader.Initialize();

    Unit wood;
    Unit target;
    Unit light;

    wood.Initialize(&woodShader);

    target.Initialize(
        &targetShader,
        L"..\\Assets\\Textures\\BrickWall.dds",
        L"..\\Assets\\Textures\\Flowers.dds");

    light.Initialize(&lightShader);

    wood.SetScale(WOOD_SCALE);
    wood.SetPosition(WOOD_CENTER);
    wood.SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

    target.SetScale(COLOR_SCALE);
    target.SetPosition(TARGET_START);
    target.SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

    light.SetScale(LIGHT_SCALE);
    light.SetPosition(LIGHT_START);
    light.SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

    targetShader.SetColorByPosition(true);

    lightShader.SetIsLightCube(true);
    lightShader.SetDiffuseColor(
        XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

    woodShader.SetDiffuseColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    woodShader.SetAmbientLight(
        XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));

    woodShader.SetLightColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    // Lower power produces a wider highlight.
    woodShader.SetSpecularPower(16.0f);

    XMFLOAT4 cameraPosition;

    XMStoreFloat4(
        &cameraPosition,
        XMMatrixInverse(
            nullptr,
            m_camera->GetView()).r[3]);

    woodShader.SetCameraPosition(cameraPosition);

    float* background = m_renderer->GetBGColor();

    background[0] = 0.0f;
    background[1] = 0.0f;
    background[2] = 0.0f;
    background[3] = 1.0f;

    SetWindowTextW(
        m_windowC->GetHWND(),
        L"DirectX12 Midterm | Arrows: X/Y | Z/X: depth");

    MidtermScene scene(wood, target, light);

    using Clock = std::chrono::steady_clock;
    auto previousTime = Clock::now();

    bool running = true;

    while (running)
    {
        MSG message = {};

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
            break;

        const auto currentTime = Clock::now();

        const float deltaTime = (std::min)(
            std::chrono::duration<float>(
                currentTime - previousTime).count(),
            0.05f);

        previousTime = currentTime;

        const Mode nextMode =
            static_cast<Mode>(m_toolsC->GetMode());

        if (nextMode != scene.mode)
        {
            if (nextMode == Mode::ColorByPosition)
            {
                target.SetPosition(TARGET_START);
                target.SetScale(COLOR_SCALE);
            }
            else if (nextMode == Mode::MoveToCube)
            {
                target.SetPosition(CHASE_START);
                target.SetScale(CHASE_SCALE);

                light.SetPosition(LIGHT_START);
            }

            scene.mode = nextMode;
        }

        wood.SetPosition(
            scene.mode == Mode::MoveToCube
            ? WOOD_CHASE_POSITION
            : WOOD_CENTER);

        if (m_toolsC->ConsumeResetLight())
        {
            light.SetPosition(LIGHT_START);
        }

        if (m_toolsC->ConsumeResetCube())
        {
            target.SetPosition(
                scene.mode == Mode::MoveToCube
                ? CHASE_START
                : TARGET_START);
        }

        const bool active =
            GetForegroundWindow() == m_windowC->GetHWND();

        auto pressed = [active](int key)
            {
                return active &&
                    (GetAsyncKeyState(key) & 0x8000) != 0;
            };

        if (pressed(VK_ESCAPE))
            break;

        // Move the light in mode 0.
        // Move the textured cube in modes 1 and 2.
        Unit& selected =
            scene.mode == Mode::MoveLight
            ? light
            : target;

        auto position = selected.GetPosition();

        const float movement = 1.1f * deltaTime;

        position.x +=
            (float(pressed(VK_RIGHT)) -
                float(pressed(VK_LEFT))) * movement;

        position.y +=
            (float(pressed(VK_UP)) -
                float(pressed(VK_DOWN))) * movement;

        position.z +=
            (float(pressed('X')) -
                float(pressed('Z'))) * movement;

        // The red light cube can move beyond the screen.
        

        selected.SetPosition(position);

        if (scene.mode == Mode::MoveToCube)
        {
            const auto targetPosition =
                target.GetPosition();

            auto lightPosition =
                light.GetPosition();

            const XMVECTOR offset = XMVectorSubtract(
                XMLoadFloat3(&targetPosition),
                XMLoadFloat3(&lightPosition));

            const float distance =
                XMVectorGetX(XMVector3Length(offset));

            if (distance > 1e-6f)
            {
                const float step =
                    (std::min)(0.75f * deltaTime, distance);

                const XMVECTOR nextPosition = XMVectorAdd(
                    XMLoadFloat3(&lightPosition),
                    XMVectorScale(offset, step / distance));

                XMStoreFloat3(
                    &lightPosition,
                    nextPosition);

                light.SetPosition(lightPosition);
            }

            if (CubesTouch(light, target))
            {
                light.SetPosition(LIGHT_START);
            }
        }

        woodShader.SetLightPosition(
            light.GetPosition());

        woodShader.SetSpecularStrength(
            m_toolsC->GetSpecularStrength() * 5.0f);

        const XMFLOAT4 selectedColor =
            m_toolsC->GetSpecularColor();

        woodShader.SetSpecularColor(selectedColor);

        // RGB also colors the illuminated surface,
        // as requested for the top face.
        woodShader.SetLightColor(
            XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

        m_renderer->Render(
            &scene,
            m_camera->GetView() *
            m_camera->GetProjection());
    }

    m_renderer->WaitForPreviousFrame();
    m_toolsC->CloseForm();
}