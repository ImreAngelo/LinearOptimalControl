#include "MainWindow.h"
#include "LinearProgram.h"
#include "RungeKutta.h"
#include "Color.h"
#include <imgui.h>

constexpr int steps = 200;

void Rendering::MainWindow::render()
{
    ImGui::Begin("Main Window");

    ImGui::BeginGroup();
    ImGui::PushStyleColor(ImGuiCol_Button, Color::BACKGROUND);

    if (ImGui::Button("Problem 1")) {
        const auto ph = Eigen::MatrixXd::Constant(1, 1, 1.0);
        const auto F0 = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return  0.0; });
        const auto Fu = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return -1.0; });

        const auto [ control, state ] = Linear::solve_t(0, 2, F0(0,0), F0, Fu, steps, ph);

        frame = PlotFrame("Example", 0, 2, control[0], state[0]);
        show = true;

#ifdef _DEBUG
        // print csv for paper 
        std::cout << "\nControl:\n" << "x, y" << std::endl;
        for (auto i = 0; i < control[0].size(); i++)
            std::cout << (2.0/steps) * i << ", " << std::round(control[0][i] * 10000) / 10000 << std::endl;

        std::cout << "\nObjective:\n" << "x, y" << std::endl;
        for (auto i = 0; i < state[0].size(); i++)
            std::cout << (2.0/steps) * i << ", " << std::round(state[0][i] * 10000) / 10000 << std::endl;
#endif // _DEBUG
    }

    ImGui::SameLine();

    if (ImGui::Button("Problem 2")) {
        const auto ph = Eigen::MatrixXd::Constant(1, 1, 1.0);
        const auto Fy = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return .7; });
        const auto Fu = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return -1; });
        const auto Fc = [](double t) { return .1*t; };

        const auto [ control, state ] = Linear::solve_t(0, 3, Fc, Fy, Fu, steps, ph);

        frame = PlotFrame("Example", 0, 3, control[0], state[0]);
        show = true;

#ifdef _DEBUG
        // print csv for paper 
        std::cout << "\nControl:\n" << "x, y" << std::endl;
        for (auto i = 0; i < control[0].size(); i++)
            std::cout << (3.0/steps) *i << ", " << std::round(control[0][i] * 10000) / 10000 << std::endl;

        std::cout << "\nObjective:\n" << "x, y" << std::endl;
        for (auto i = 0; i < state[0].size(); i++)
            std::cout << (3.0/steps) * i << ", " << std::round(state[0][i] * 10000)/10000 << std::endl;
#endif // _DEBUG
    }

    ImGui::SameLine();

    if (ImGui::Button("Problem 3")) {
        const auto Fy = Eigen::Matrix<std::function<double(double)>, 2, 2>::Constant([](double t) { return 0; });
        const auto Fc = [](double t) { return 0; };

        Eigen::Matrix<std::function<double(double)>, 2, 2> Fu;
        Eigen::Matrix<double, 1, 2> phi;

        Fu << [](double t) { return -1.0; }, [](double t) { return 0.0; },
              [](double t) { return 0.0; }, [](double t) { return 1.0; };

        phi << 0.0, 1.0;

        const double t0 = 0;
        const double t1 = 1.0;

        auto [control, state] = Linear::solve_t(t0, t1, Fc, Fy, Fu, 200, phi, 1);

        frame = PlotFrame("Example", t0, t1, control[1], state[1]);
        show = true;

        //#ifdef _DEBUG
        //        // print csv for paper 
        //        std::cout << "\nControl:\n" << "x, y" << std::endl;
        //        for (auto i = 0; i < solution.control[0].size(); i++)
        //            std::cout << (3.0/steps) *i << ", " << std::round(solution.control[0][i] * 10000) / 10000 << std::endl;
        //
        //        std::cout << "\nObjective:\n" << "x, y" << std::endl;
        //        for (auto i = 0; i < solution.objective[0].size(); i++)
        //            std::cout << (3.0/steps) * i << ", " << std::round(solution.objective[0][i] * 10000)/10000 << std::endl;
        //#endif // _DEBUG

        x = control[0];
        y = state[0];
        time = std::vector<double>(x.size(), 0.0);

        for (auto i = 0; i < x.size(); i++)
            time[i] = ((t1 - t0) / steps) * i;
    }

    RungeKutta::ButcherTable butcherTable = (RungeKutta::debug == 0) ? RungeKutta::euler :
                                            (RungeKutta::debug == 1) ? RungeKutta::heun : RungeKutta::rk4;

    if (ImGui::Button("Timing Test #1"))
    {
        Eigen::Matrix<std::function<double(double)>, 1, 1> F0, Fy;
        auto y0 = Eigen::MatrixXd::Constant(1, 1, 1.0);

        F0 << [](double t) { return 0.1*t; };
        Fy << [](double t) { return -1.0; };

        auto [r, t] = RungeKutta::solve(y0, F0, Fy, F0, steps, 5.0, 0.0, butcherTable);

        x = std::vector<double>(steps, 0.0);
        y = std::vector<double>(steps, 0.0);

        for (auto i = 0; i < steps; i++)
        {
            x[i] = r[i](0, 0);
            y[i] = 0;
        }

        time = t;
    }

    ImGui::SameLine();

    if (ImGui::Button("Timing Test #2"))
    {
        constexpr double a = 1, b = 2, c = 3, d = 1;

        Eigen::Matrix<std::function<double(double)>, 2, 2> F0, Fy;
        auto y0 = Eigen::Matrix2d::Constant(2, 2, 1.0);

        F0 << [](double t) { return 0.0; }, [](double t) { return 0.0; },
              [](double t) { return 0.0; }, [](double t) { return 0.0; };

        Fy << [](double t) { return a; }, [](double t) { return -b; },
              [](double t) { return c; }, [](double t) { return -d; };

        auto [ r, t ] = RungeKutta::solve(y0, F0, Fy, F0, steps, 5.0, 0.0, butcherTable);

        x = std::vector<double>(steps, 0.0);
        y = std::vector<double>(steps, 0.0);

        for (auto i = 0; i < steps; i++)
        {
            x[i] = r[i](0, 0);
            y[i] = r[i](1, 0);
        }

        time = t;
    }

    // DEBUGGING
    if (ImGui::Button("Eulers Method")) { RungeKutta::debug = 0; }
    ImGui::SameLine();
    if (ImGui::Button("Heun's Method")) { RungeKutta::debug = 1; }
    ImGui::SameLine();
    if (ImGui::Button("Classic RK4")) { RungeKutta::debug = 2; }

    ImGui::PopStyleColor();
    ImGui::EndGroup();

    ImGui::End();

    if (show) {
        frame.render();
    } 

    if (time.size() > 0)
    {
        ImGui::Begin("Result");

        if (ImPlot::BeginPlot("Runge-Kutta 2D"))
        {
            ImPlot::PushStyleColor(ImPlotCol_Line, Color::DYNAMIC);
            ImPlot::PushStyleColor(ImPlotCol_Line, Color::CONTROL);
            ImPlot::PlotLine("x(t)", &time[0], &x[0], time.size());
            ImPlot::PopStyleColor();
            ImPlot::PlotLine("y(t)", &time[0], &y[0], time.size());
            ImPlot::PopStyleColor();
            ImPlot::EndPlot();
        }

        ImGui::End();
    }
}
