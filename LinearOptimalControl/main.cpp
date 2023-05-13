#include "Application.h"

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

int main()
{
#ifdef _WIN32
#ifdef _DEBUG
    ShowWindow(GetConsoleWindow(), SW_SHOW);
#else
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif // _DEBUG
#endif // _WIN32

    // Draw UI on main thread - TODO: Spin off optimization to separate thread
    Rendering::Application window("Linear Optimal Control Solver");
    while (!window.shouldClose()) {
        window.update();
    }

	return 0;
}