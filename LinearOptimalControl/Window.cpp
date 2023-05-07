#include "Window.h"
#include <GLFW/glfw3.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "implot.h"

#include "PlotFrame.h"

using namespace Rendering;

void Window::update()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Poll for events
    glfwPollEvents();

    // Start a new dear ImGUI frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Draw ImGUI - TODO: Show all windows from list
    ImGui::Begin("Window Here");
    ImGui::Text("Dis tsome text");

    PlotFrame frame;
    frame.drawPlot("Plot");

    ImGui::Text("More text");

    // Render dear ImGUI
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Multi-Viewport
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    // Swap buffers
    glfwSwapBuffers(window);
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(window);
}

Window::Window(const char* title, bool showWindow)
{
    // Initialize GLFW
    glfwInit();

    // Create a window
    glfwWindowHint(GLFW_VISIBLE, showWindow);
    window = glfwCreateWindow(1920, 1080, title, NULL, NULL);

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize dear ImGUI
    ImGui::CreateContext();
    PlotFrame temp;
    auto colors = temp.colors;
    glClearColor(colors.bk.x, colors.bk.y, colors.bk.z, colors.bk.w);

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    if(!showWindow)
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Initialize ImPlot
    ImPlot::CreateContext();
}

Window::~Window()
{
    // Clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}
