#include <Windows.h>
#include "Window.h"

int main()
{
    // Hide console
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    Window window("Linear Optimal Control Solver");
    while(!window.shouldClose()) {
        window.update();
    }

	return 0;
}