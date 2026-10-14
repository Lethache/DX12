#pragma once

#include "StandardIncludes.h"

class GameController : public Singleton<GameController>
{
public:
    GameController();
    virtual ~GameController();

    void Initialize(HINSTANCE hInstance, int nShowCmd);
    void Run();

private:
    WindowController* m_windowC;
    Renderer* m_renderer;
    Camera* m_camera;
    ToolsController* m_toolsC;
};