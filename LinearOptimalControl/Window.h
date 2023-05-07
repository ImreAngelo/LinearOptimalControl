#pragma once

#include "imgui.h"

extern struct GLFWwindow;

/** TODO:
 * - Use custom title bar /w close & minimize (hide native)
 * - C++ify color (make macros into const_expr in new file)
 */

namespace Rendering
{
    /// <summary>
    /// Creates an application window
    /// </summary>
    class Window
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
        Window(const char* title, bool show = true);

        ~Window();

    private:
        GLFWwindow* window;
    };
}

