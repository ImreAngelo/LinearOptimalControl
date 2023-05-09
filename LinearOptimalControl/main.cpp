#include "Application.h"

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

int main()
{
#ifdef _DEBUG
#ifdef _WIN32
    ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif // _WIN32
#elif _WIN32
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif // _DEBUG

    // Draw UI on main thread
    Rendering::Application window("Linear Optimal Control Solver");
    while (!window.shouldClose()) {
        window.update();
    }

	return 0;
}