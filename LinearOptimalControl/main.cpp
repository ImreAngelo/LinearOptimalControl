#include "Application.h"

#ifdef _WIN32
#include <Windows.h>

// Show window in debug and timing mode
void initSystem(bool release = false)
{
#if defined(_DEBUG) || defined(TIMING)
    ShowWindow(GetConsoleWindow(), SW_SHOW);
#else
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif // _DEBUG || TIMING
}

#elif
// If not on windows
void initSystem(bool release = false);
#endif // _WIN32

int main()
{
    // Hide console on windows
    initSystem();

    // Draw UI on main thread - TODO: Spin off optimization to separate thread
    Rendering::Application window("Linear Optimal Control Solver");
    while (!window.shouldClose()) {
        window.update();
    }

	return 0;
}