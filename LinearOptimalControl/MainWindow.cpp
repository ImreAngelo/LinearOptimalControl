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
        auto F0 = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return  0.0; });
        auto Fu = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return -1.0; });

        auto solution = Linear::solve_t(0, 2, F0(0), F0, Fu, steps);

        frame = PlotFrame("Example 1", 0, 2, solution.control[0], solution.objective[0]);
        show = true;

#ifndef _DEBUG
        // print csv for paper 
        std::cout << "\nControl:\n" << "x, y" << std::endl;
        for (auto i = 0; i < solution.control[0].size(); i++)
            std::cout << (2.0/steps) * i << ", " << std::round(solution.control[0][i] * 10000) / 10000 << std::endl;

        std::cout << "\nObjective:\n" << "x, y" << std::endl;
        for (auto i = 0; i < solution.objective[0].size(); i++)
            std::cout << (2.0/steps) * i << ", " << std::round(solution.objective[0][i] * 10000) / 10000 << std::endl;
#endif // _DEBUG
    }

    ImGui::SameLine();

    if (ImGui::Button("Problem 2")) {
        auto Fy = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return -.7; });
        auto Fu = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return -1.; });
        auto Fc = [](double t) { return .1*t; };

        auto solution = Linear::solve_t(0, 3, Fc, Fy, Fu, steps);

        frame = PlotFrame("Example 2", 0, 3, solution.control[0], solution.objective[0]);
        show = true;

#ifdef _DEBUG
        // print csv for paper 
        std::cout << "\nControl:\n" << "x, y" << std::endl;
        for (auto i = 0; i < solution.control[0].size(); i++)
            std::cout << (3.0/steps) *i << ", " << std::round(solution.control[0][i] * 10000) / 10000 << std::endl;

        std::cout << "\nObjective:\n" << "x, y" << std::endl;
        for (auto i = 0; i < solution.objective[0].size(); i++)
            std::cout << (3.0/steps) * i << ", " << std::round(solution.objective[0][i] * 10000)/10000 << std::endl;
#endif // _DEBUG
    }

    ImGui::SameLine();
    if (ImGui::Button("Problem 3")) {
        auto Fc = [](double t) { return 0.0; };
        Eigen::Matrix<std::function<double(double)>, 2, 2> Fy;
        Eigen::Matrix<std::function<double(double)>, 2, 2> Fu;

        Fy << [](double t) { return 0.0; }, [](double t) { return 0.0; }, 
              [](double t) { return 0.0; }, [](double t) { return 0.0; };

        Fu << [](double t) { return -1.0; }, [](double t) { return 0.0; },
              [](double t) { return 0.0; },  [](double t) { return 1.0; };

        auto solution = Linear::solve_t(0, 3, Fc, Fy, Fu, steps);

        frame = PlotFrame("Example 3", 0, 3, solution.control[0], solution.objective[0]);
        show = true;

//#ifdef _DEBUG
//        // print csv for paper 
//        std::cout << "\nControl 1:\n" << "x, y" << std::endl;
//        for (auto i = 0; i < solution.control.size(); i++)
//            std::cout << (2.0 / steps) * i << ", " << std::round(solution.control[i] * 10000) / 10000 << std::endl;
//
//        std::cout << "\nObjective 1:\n" << "x, y" << std::endl;
//        for (auto i = 0; i < solution.objective.size(); i++)
//            std::cout << (2.0 / steps) * i << ", " << std::round(solution.objective[i] * 10000) / 10000 << std::endl;
//#endif // _DEBUG
    }

    ImGui::PopStyleColor();
    ImGui::EndGroup();

    ImGui::End();

    if (show) {
        frame.render();
    }
}
