#include <Windows.h>
#include <thread>
#include "Window.h"

int main()
{
    // Hide console
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Draw UI on main thread
    Rendering::Window window("Linear Optimal Control Solver");
    while (!window.shouldClose()) {
        window.update();
    }

    // 

	return 0;
}