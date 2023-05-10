#pragma once

#include "imgui.h"
#include "Window.h"
#include <vector>

struct GLFWwindow;

/** TODO:
 * - Use custom title bar /w close & minimize (hide native)
 */
namespace Rendering
{
    /// <summary>
    /// Creates an application window
    /// </summary>
    class Application
    {
    public:
        /// <summary>
        /// Render window content
        /// </summary>
        void update();

        /// <summary>
        /// Check if window has been closed
        /// </summary>
        /// <returns>Boolean</returns>
        bool shouldClose();

        /// <summary>
        /// Create a new application/window
        /// </summary>
        /// <param name="title">Name of the window</param>
        /// <param name="showWindow">Experimental</param>
        Application(const char* title);

        ~Application();

    private:
        GLFWwindow* window;

        Rendering::Window* mainWindow;
        //std::vector<Rendering::Window> windows;
    };
}

