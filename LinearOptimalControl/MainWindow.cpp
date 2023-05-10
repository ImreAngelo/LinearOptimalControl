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

#ifdef _DEBUG
        // print csv for paper 
        std::cout << "\nControl:\n" << "x, y" << std::endl;
        for (auto i = 0; i < solution.control.size(); i++)
            std::cout << (2.0/steps) * i << ", " << std::round(solution.control[i] * 10000) / 10000 << std::endl;

        std::cout << "\nObjective:\n" << "x, y" << std::endl;
        for (auto i = 0; i < solution.objective.size(); i++)
            std::cout << (2.0/steps) * i << ", " << std::round(solution.objective[i] * 10000) / 10000 << std::endl;
#endif // _DEBUG
    }

    ImGui::SameLine();

    if (ImGui::Button("Problem 2")) {
        constexpr double t1 = 3.0;
        constexpr double dt = t1/steps;

        const auto Fy = Eigen::VectorXd::Constant(steps, 1, .7);
        const auto Fu = Eigen::VectorXd::Constant(steps, 1, -1.0);
        
        Eigen::VectorXd Fc{steps};
        for (auto i = 0; i < steps; i++) {
            Fc(i,0) = dt * i * .1;
        }

        auto solution = Linear::solve(0, t1, Fc, Fy, Fu);
        frame = PlotFrame("Example 2", 0, t1, solution.control, solution.objective);

        show = true;

#ifdef _DEBUG
        // print csv for paper 
        std::cout << "\nControl:\n" << "x, y" << std::endl;
        for (auto i = 0; i < solution.control.size(); i++)
            std::cout << dt * i << ", " << std::round(solution.control[i] * 10000)/10000 << std::endl;

        std::cout << "\nObjective:\n" << "x, y" << std::endl;
        for (auto i = 0; i < solution.objective.size(); i++)
            std::cout << dt * i << ", " << std::round(solution.objective[i] * 10000)/10000 << std::endl;
#endif // _DEBUG
    }

    ImGui::SameLine();
    if (ImGui::Button("Problem 3")) {
        constexpr double t1 = 3.0;
        constexpr double dt = t1 / steps;

        const Eigen::MatrixXd F0 = Eigen::MatrixXd::Constant(steps, 2, 0);
        const Eigen::MatrixXd Fu(steps, 2);

        auto solution = Linear::solve(0, t1, F0, F0, Fu);
        frame = PlotFrame("Example 2", 0, t1, solution.control, solution.objective);

        show = true;

#ifdef _DEBUG
        // print csv for paper 
        std::cout << "\nControl:\n" << "x, y" << std::endl;
        for (auto i = 0; i < solution.control.size(); i++)
            std::cout << (2.0 / steps) * i << ", " << std::round(solution.control[i] * 10000) / 10000 << std::endl;

        std::cout << "\nObjective:\n" << "x, y" << std::endl;
        for (auto i = 0; i < solution.objective.size(); i++)
            std::cout << (2.0 / steps) * i << ", " << std::round(solution.objective[i] * 10000) / 10000 << std::endl;
#endif // _DEBUG
    }

    ImGui::PopStyleColor();
    ImGui::EndGroup();

    ImGui::End();

    if (show) {
        frame.render();
    }
}
