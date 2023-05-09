#include "Application.h"

#ifndef _DEBUG
#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32
#endif // _DEBUG

int main()
{
#ifndef _DEBUG
#ifdef _WIN32
    // Hide console
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif // _WIN32
#endif // _DEBUG

    // Draw UI on main thread
    Rendering::Application window("Linear Optimal Control Solver");
    while (!window.shouldClose()) {
        window.update();
    }

	return 0;
}