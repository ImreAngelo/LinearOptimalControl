#include "MainWindow.h"
#include "Color.h"
#include <imgui.h>

void Rendering::MainWindow::render()
{
    ImGui::Begin("Main Window");

    ImGui::BeginGroup();
    ImGui::PushStyleColor(ImGuiCol_Button, Color::BACKGROUND);

    if (ImGui::Button("Problem 1")) {
        lp.solve(0, 2, 20);
        frame = PlotFrame("Example 1", lp.getControl(), lp.getDynamics());
        show = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Problem 2")) {
        lp.solve(0, 2, 200);
        frame = PlotFrame("Example 2", lp.getControl(), lp.getDynamics());
        show = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Problem 3")) {
        lp.solve(0, 1, 50);
        frame = PlotFrame("Example 3", lp.getControl(), lp.getDynamics());
        show = true;
    }

    ImGui::PopStyleColor();
    ImGui::EndGroup();

    ImGui::End();

    if (show) {
        frame.render();
    }
}
