#include "Window.h"
#include <GLFW/glfw3.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "implot.h"

#include "PlotFrame.h"
#include "Color.h"

using namespace Rendering;

void temp(Window* w)
{
    ImGui::Begin("Main Window");

    //ImGui::BeginGroup();
    //if (ImGui::Button("Save"))
    //{
    //    //... my_code 
    //}
    //ImGui::SameLine();
    //if (ImGui::Button("Cancel"))
    //{
    //    //... my_code 
    //}
    //ImGui::EndGroup();

    ImGui::BeginGroup();
    ImGui::PushStyleColor(ImGuiCol_Button, Color::BACKGROUND);

    if (ImGui::Button("Problem 1")) {

    }

    //if (ImGui::Button("Problem 2")) { }
    //if (ImGui::Button("Problem 3")) { }

    ImGui::PopStyleColor();
    ImGui::EndGroup();

    //ImGui::Text("More text");
    ImGui::End();
}

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
    temp(this);
    PlotFrame frame1, frame2;
    frame1.drawPlot("Plot 1");
    frame2.drawPlot("Plot 2");

    // Render dear ImGUI
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
    glClearColor(Color::BACKGROUND.x, Color::BACKGROUND.y, Color::BACKGROUND.z, Color::BACKGROUND.w);

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
