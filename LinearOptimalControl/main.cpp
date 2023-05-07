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
    GLFWwindow* window = glfwCreateWindow(640, 480, "My Window", NULL, NULL);

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize dear ImGUI and ImPlot
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Plotting
    ImPlot::CreateContext();

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll for events
        glfwPollEvents();

        // Start a new dear ImGUI frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


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

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Clean up
    //ImPlot::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
	return 0;
}