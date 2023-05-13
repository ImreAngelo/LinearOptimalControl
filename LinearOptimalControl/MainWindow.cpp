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
        Eigen::Matrix<std::function<double(double)>, 2, 2> F0 = Eigen::Matrix<std::function<double(double)>, 2, 2>::Constant([](double t) { return 0.0; });
        Eigen::Matrix<std::function<double(double)>, 2, 2> Fu;

        auto Fc = [](double t) { return 0.0; };
        
        Fu << [](double t) { return -1.0; }, [](double t) { return 0.0; },
              [](double t) { return 0.0; }, [](double t) { return -1.0; };

        auto solution = Linear::solve_t(0, 2, Fc, F0, Fu, steps);

        frame = PlotFrame("Example 1", 0, 2, solution.control[0], solution.objective[0]);
        show = true;

//        const auto F0 = Eigen::VectorXd::Constant(steps, 1, 0.0);
//        const auto Fu = Eigen::VectorXd::Constant(steps, 1, -1.0);
//        const auto solution = Linear::solve(0, 2, F0, F0, Fu);
//
//        frame = PlotFrame("Example 1", 0, 2, solution.control, solution.objective);
//        show = true;
//
//#ifdef _DEBUG
//        // print csv for paper 
//        std::cout << "\nControl:\n" << "x, y" << std::endl;
//        for (auto i = 0; i < solution.control.size(); i++)
//            std::cout << (2.0/steps) * i << ", " << std::round(solution.control[i] * 10000) / 10000 << std::endl;
//
//        std::cout << "\nObjective:\n" << "x, y" << std::endl;
//        for (auto i = 0; i < solution.objective.size(); i++)
//            std::cout << (2.0/steps) * i << ", " << std::round(solution.objective[i] * 10000) / 10000 << std::endl;
//#endif // _DEBUG
    }

    ImGui::SameLine();

    if (ImGui::Button("Problem 2")) {
        Eigen::Matrix<std::function<double(double)>, 2, 2> Fy;
        Eigen::Matrix<std::function<double(double)>, 2, 2> Fu;

        auto Fc = [](double t) { return 1.0; };

        Fy << [](double t) { return 0.7; }, [](double t) { return 0.0; },
              [](double t) { return 0.0; }, [](double t) { return -0.7; };

        Fu << [](double t) { return -1.0; }, [](double t) { return 0.0; },
              [](double t) { return 0.0; }, [](double t) { return -1.0; };

        auto solution = Linear::solve_t(0, 2, Fc, Fy, Fu, steps);

        frame = PlotFrame("Example 2", 0, 2, solution.control[1], solution.objective[1]);
        show = true;

//        constexpr double t1 = 3.0;
//        constexpr double dt = t1/steps;
//
//        const auto Fy = Eigen::VectorXd::Constant(steps, 1, .7);
//        const auto Fu = Eigen::VectorXd::Constant(steps, 1, -1.0);
//        
//        Eigen::VectorXd Fc{steps};
//        for (auto i = 0; i < steps; i++) {
//            Fc(i,0) = dt * i * .1;
//        }
//
//        auto solution = Linear::solve(0, t1, Fc, Fy, Fu);
//        frame = PlotFrame("Example 2", 0, t1, solution.control, solution.objective);
//
//        show = true;
//
//#ifdef _DEBUG
//        // print csv for paper 
//        std::cout << "\nControl:\n" << "x, y" << std::endl;
//        for (auto i = 0; i < solution.control.size(); i++)
//            std::cout << dt * i << ", " << std::round(solution.control[i] * 10000)/10000 << std::endl;
//
//        std::cout << "\nObjective:\n" << "x, y" << std::endl;
//        for (auto i = 0; i < solution.objective.size(); i++)
//            std::cout << dt * i << ", " << std::round(solution.objective[i] * 10000)/10000 << std::endl;
//#endif // _DEBUG
    }

    ImGui::SameLine();
    if (ImGui::Button("Problem 3")) {
        /*auto F0 = Eigen::Matrix<std::function<double(double)>, 2, 2>::Constant(2,2, [](double t) { return -1.0; });
        Eigen::Matrix<std::function<double(double)>, 2, 2> Fu;

        Fu << [](double t) { return -1.0; }, [](double t) { return 0.0; },
              [](double t) { return 0.0; }, [](double t) { return 1.0; };

        auto solution = Linear::solve_t(0, 2, Fc, Fy, Fu, steps);

        frame = PlotFrame("Example 3", 0, 2, solution.control[1], solution.objective[1]);
        show = true;*/

//        constexpr double t1 = 3.0;
//        constexpr double dt = t1 / steps;
//
//        const Eigen::MatrixXd F0 = Eigen::MatrixXd::Constant(2, 2, 0);
//        Eigen::MatrixXd Fu(2, 2);
//        Fu << 1, 1, 
//              0, 1;
//
//        auto solution = Linear::solve(0, t1, F0, F0, Fu);
//        frame = PlotFrame("Example 2", 0, t1, solution.control, solution.objective);
//
//        show = true;
//
//#ifdef _DEBUG
//        // print csv for paper 
//        std::cout << "\nControl:\n" << "x, y" << std::endl;
//        for (auto i = 0; i < solution.control.size(); i++)
//            std::cout << (2.0 / steps) * i << ", " << std::round(solution.control[i] * 10000) / 10000 << std::endl;
//
//        std::cout << "\nObjective:\n" << "x, y" << std::endl;
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
