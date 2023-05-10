#include "MainWindow.h"
#include "LinearProgram.h"
#include "Color.h"
#include <imgui.h>

constexpr int steps = 200;

void Rendering::MainWindow::render()
{
    ImGui::Begin("Main Window");

    ImGui::BeginGroup();
    ImGui::PushStyleColor(ImGuiCol_Button, Color::BACKGROUND);

    if (ImGui::Button("Problem 1")) {
        const auto F0 = Eigen::VectorXd::Constant(steps, 1, 0.0);
        const auto Fu = Eigen::VectorXd::Constant(steps, 1, -1.0);
        const auto solution = Linear::solve(0, 2, F0, F0, Fu);

        frame = PlotFrame("Example 1", 0, 2, solution.control, solution.objective);

        show = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Problem 2")) {
        constexpr double t1 = 3.0;
        constexpr double dt = t1/steps;

        const auto Fy = Eigen::VectorXd::Constant(steps, 1, 0.0);
        const auto Fu = Eigen::VectorXd::Constant(steps, 1, -1.0);
        
        Eigen::VectorXd Fc{steps};
        for (auto i = 0; i < steps; i++) {
            Fc(i,0) = dt * i;
        }

        auto solution = Linear::solve(0, t1, Fc, Fy, Fu);
        frame = PlotFrame("Example 2", 0, t1, solution.control, solution.objective);

        show = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Problem 3")) {
        /*lp.solve(0, 1, 500);
        frame = PlotFrame("Example 3", lp.getControl(), lp.getDynamics());*/
        show = true;
    }

    ImGui::PopStyleColor();
    ImGui::EndGroup();

    ImGui::End();

    if (show) {
        frame.render();
    }
}
