#include <Windows.h>
#include "DemoApp.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    DemoApp Game(hInstance);

    Game.Initialize(1024, 768);
    Game.Run();

    return 0;
}
