#pragma once

#include "StandardIncludes.h"

class GameController : public Singleton<GameController>
{
public:
    // Constructors / Destructors
    GameController();
    virtual ~GameController();

    // Methods
    void Initialize(
        HINSTANCE _hInstance,
        int _nShowCmd);

    void Run();

private:
    WindowController* m_windowC;
    Renderer* m_renderer;
};