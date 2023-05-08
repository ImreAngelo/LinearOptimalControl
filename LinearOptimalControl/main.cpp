#include <Windows.h>
#include "Application.h"

int main()
{
    // Hide console
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Draw UI on main thread
    Rendering::Application window("Linear Optimal Control Solver");
    while (!window.shouldClose()) {
        window.update();
    }

	return 0;
}