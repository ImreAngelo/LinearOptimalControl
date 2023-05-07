#pragma once
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "implot.h"
#include <math.h>

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

    void drawPlot(const char* windowName)
    {
        if (ImPlot::BeginPlot(windowName))
        {
            // Plot a line graph
            static float data[100];
            for (int i = 0; i < 100; i++)
                data[i] = sin(i * 0.1f);

            ImPlot::PlotLine("Control", data, 100);
            ImPlot::EndPlot();
        }
    }
};

