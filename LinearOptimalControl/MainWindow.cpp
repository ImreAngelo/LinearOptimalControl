#include "MainWindow.h"
#include "LinearProgram.h"
#include "RungeKutta.h"
#include "Color.h"
#include <imgui.h>

void Rendering::MainWindow::render()
{
    ImGui::Begin("Menu");

    ImGui::BeginGroup();
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Color::FOREGROUND);
    ImGui::PushStyleColor(ImGuiCol_Button, Color::FOREGROUND);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, Color::FOREGROUND);

    // ===== Example Problems

    ImGui::Dummy(ImVec2(0.0f, 0.0f));
    ImGui::Text("Examples");
    ImGui::Dummy(ImVec2(0.0f, 2.0f));

    if (ImGui::Button("Problem 1")) {
        t0 = 0;
        t1 = 2;

        const auto ph = Eigen::MatrixXd::Constant(1, 1, 1.0);
        const auto F0 = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return  0.0; });
        const auto Fu = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return -1.0; });

        const auto [ ut, yt ] = Linear::solve_t(t0, t1, RungeKutta::getTable(method), F0(0,0), F0, Fu, steps, ph);
        control = ut; state = yt; show = true;
    }

    ImGui::SameLine();

    if (ImGui::Button("Problem 2")) {
        t0 = 0;
        t1 = 3;

        const auto ph = Eigen::MatrixXd::Constant(1, 1, 1.0);
        const auto Fy = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return .7; });
        const auto Fu = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return -1; });
        const auto Fc = [](double t) { return .1*t; };

        const auto [ut, yt] = Linear::solve_t(t0, t1, RungeKutta::getTable(method), Fc, Fy, Fu, steps, ph);
        control = ut; state = yt; show = true;
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

        auto [ ut, yt ] = Linear::solve_t(t0, t1, RungeKutta::getTable(method), Fc, Fy, Fu, steps, phi, 1);
        control = ut; state = yt; show = true;

        x = ut[1];
        y = yt[1];

        time = std::vector<double>(x.size(), 0.0);
        for (auto i = 0; i < x.size(); i++)
            time[i] = ((t1 - t0) / steps) * i;
    }

    // ===== Timing Tests (Standard RK)

    if (ImGui::Button("Timing Test #1"))
    {
        Eigen::Matrix<std::function<double(double)>, 1, 1> F0, Fy;
        auto y0 = Eigen::MatrixXd::Constant(1, 1, 1.0);

        F0 << [](double t) { return 0.1*t; };
        Fy << [](double t) { return -1.0; };

        auto [r, t] = RungeKutta::solve(y0, F0, Fy, F0, steps, 5.0, 0.0, RungeKutta::getTable(method));

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

        auto [ r, t ] = RungeKutta::solve(y0, F0, Fy, F0, steps, 5.0, 0.0, RungeKutta::getTable(method));

        x = std::vector<double>(steps, 0.0);
        y = std::vector<double>(steps, 0.0);

        for (auto i = 0; i < steps; i++)
        {
            x[i] = r[i](0, 0);
            y[i] = r[i](1, 0);
        }

        time = t;
    }

    // ===== Runge-Kutta Method

    const char* items[] = { "Euler's", "Implicit Euler", "Heun's 2nd Order", "Classic RK4" };

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Text("Runge-Kutta Options");
    ImGui::Dummy(ImVec2(0.0f, 2.0f));

    if (ImGui::BeginCombo("Method", items[method]))
    {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        {
            bool is_selected = (items[method] == items[n]);
            if (ImGui::Selectable(items[n], is_selected))
                method = n;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::InputInt("Steps", &steps, 1, 25);
    steps = std::min(std::max(steps, 2), 500);

    // ===== TODO: Calculate on button press

    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::Button("Solve");
    
    // ===== Print CSV

#ifdef _DEBUG
    // Print CSV for thesis graphs
    if (ImGui::Button("Print CSV to console"))
    {
        for (auto d = 0; d < control.size(); d++)
        {
            std::cout << "\n\n == Control " << (d + 1) << " =============================\n" << std::setw(6) << "x, " << std::setw(4) << "y\n";
            for (auto i = 0; i < control[d].size(); i++)
                std::cout << std::setw(6) << (3.0 / steps) * i << ", " << std::setw(4) << std::round(control[d][i] * 10000) / 10000 << std::endl;

            std::cout << "\n\n == State " << (d + 1) << " =============================\n" << std::setw(6) << "x, " << std::setw(4) << "y\n";
            for (auto i = 0; i < state[d].size(); i++)
                std::cout << std::setw(6) << (3.0 / steps) * i << ", " << std::setw(4) << std::round(state[d][i] * 10000) / 10000 << std::endl;
        }
    }
#endif // _DEBUG

    // ===== End GUI

    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::EndGroup();

    ImGui::End();

    // ===== Render Plots

    if (show) {
        frame = PlotFrame("Example", t0, t1, control[0], state[0]);
        show = false;
    }

    if (control.size() > 0)
        frame.render();

    if (time.size() > 0)
    {
        ImGui::Begin("Result");

        if (ImPlot::BeginPlot("Runge-Kutta 2D"))
        {
            ImPlot::PushStyleColor(ImPlotCol_Line, Color::DYNAMIC);
            ImPlot::PushStyleColor(ImPlotCol_Line, Color::CONTROL);
            ImPlot::PushStyleColor(ImPlotCol_FrameBg, Color::FOREGROUND);
            ImPlot::PlotLine("x(t)", &time[0], &x[0], time.size());
            ImPlot::PopStyleColor();
            ImPlot::PlotLine("y(t)", &time[0], &y[0], time.size());
            ImPlot::PopStyleColor();
            ImPlot::PopStyleColor();
            ImPlot::EndPlot();
        }

        ImGui::End();
    }
}
