#include "GameController.h"

int WINAPI main(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    GameController* gc =
        &GameController::GetInstance();

    gc->Initialize(
        hInstance,
        nShowCmd);

    gc->Run();

    return 0;
}