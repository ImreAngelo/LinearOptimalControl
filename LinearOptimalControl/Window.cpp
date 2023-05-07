#include "Window.h"

void Window::update()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Poll for events
    glfwPollEvents();

    // Start a new dear ImGUI frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Draw ImGUI
    ImGui::Begin("Window Here");
    ImGui::Text("Dis tsome text");

    drawPlot("Plot 1");
    drawPlot("Plot 2");

    ImGui::End();

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
{// Initialize GLFW
    glfwInit();

    // Create a window
    glfwWindowHint(GLFW_VISIBLE, showWindow);
    window = glfwCreateWindow(1920, 1080, title, NULL, NULL);

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize dear ImGUI
    ImGui::CreateContext();
    glClearColor(.1f, .1f, .1f, 1.0f);

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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
