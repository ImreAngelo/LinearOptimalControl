#include "MainWindow.h"
#include "Color.h"
#include <imgui.h>

void Rendering::MainWindow::render()
{
    ImGui::Begin("Main Window");

    ImGui::BeginGroup();
    ImGui::PushStyleColor(ImGuiCol_Button, Color::BACKGROUND);

    if (ImGui::Button("Problem 1")) {
        lp.solve(0, 2, 40);
        frame = PlotFrame("Example 1", lp.getControl(), lp.getDynamics());
        //show = !show;
    }

    //ImGui::SameLine();

    ImGui::PopStyleColor();
    ImGui::EndGroup();

    ImGui::End();

    if (show) {
        frame.render();
    }
}
