#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "implot.h"
#include <math.h>
#include <Windows.h>

int main()
{
    // Hide console
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Initialize GLFW
    glfwInit();

    // Create a window
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Linear Optimal Control Solver", NULL, NULL);

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

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll for events
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start a new dear ImGUI frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Draw ImGUI
        ImGui::Begin("Window Here");
        ImGui::Text("Dis tsome text");
        
        if (ImPlot::BeginPlot("My Plot"))
        {
            // Plot a line graph
            static float data[100];
            for (int i = 0; i < 100; i++)
                data[i] = sin(i * 0.1f);

            ImPlot::PlotLine("My Plot", data, 100);
            ImPlot::EndPlot();
        }

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

    // Clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
	return 0;
}