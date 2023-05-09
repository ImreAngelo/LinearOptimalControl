#include "Application.h"
#include <GLFW/glfw3.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "implot.h"

#include "MainWindow.h"
#include "PlotFrame.h"
#include "Color.h"

using namespace Rendering;

void Application::update()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Poll for events
    glfwPollEvents();

    // Start a new dear ImGUI frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Dockspace
    ImGui::DockSpaceOverViewport();

    // Draw all windows
    mainWindow->render();
    /*
    for (auto& w : windows)
        w.render();
    */

    // Render dear ImGUI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Multi-Viewport
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    // Swap buffers
    glfwSwapBuffers(window);
}

bool Application::shouldClose()
{
    return glfwWindowShouldClose(window);
}

Application::Application(const char* title, bool showWindow)
{
    // Initialize GLFW
    glfwInit();

    // Create a window
    glfwWindowHint(GLFW_VISIBLE, showWindow);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    window = glfwCreateWindow(1280, 720, title, NULL, NULL);

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize dear ImGUI
    ImGui::CreateContext();
    glClearColor(Color::BACKGROUND.x, Color::BACKGROUND.y, Color::BACKGROUND.z, Color::BACKGROUND.w);

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //if(!showWindow)
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Initialize ImPlot
    ImPlot::CreateContext();

    // Make main window
    mainWindow = new MainWindow();
    //windows.emplace_back(PlotFrame("plot 1"));
}

Application::~Application()
{
    // Clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    delete mainWindow;
}
