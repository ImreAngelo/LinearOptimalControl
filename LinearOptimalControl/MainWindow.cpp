#include "MainWindow.h"
#include "LinearProgram.h"
#include "RungeKutta.h"
#include "Color.h"
#include <imgui.h>
#include "ExampleProblems.h"


void print(int n, int steps, int method)
{
    std::string s("");
    switch (method)
    {
    case 0: s = "Euler's method"; break;
    case 1: s = "implicit Euler's method"; break;
    case 2: s = "Heun's 2nd order method"; break;
    case 3: s = "classic RK4"; break;
    }

    std::cout << "\n\nRunning example " << n << " with " << steps << " steps of " << s << "\n";
}

double get_max_err(std::vector<double> a, std::vector<double> b)
{
    double max = 0;
    for (auto i = 0; i < a.size(); i++)
        max = std::max(max, abs(a[i] - b[i]));
    return max;
}

constexpr int stepsizes[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 120, 140, 160, 180, 200, 250, 300, 350, 400, 450, 500 };

void debug(std::function<Linear::Solution(int)> solve, std::tuple<Example::vf, Example::vf> solution, double t0 = 0, double t1 = 1)
{
#ifdef TIMING

    std::cout << "\n\nn, int, param, cplex";

    for (auto i = 0; i < IM_ARRAYSIZE(stepsizes); i++)
    {
        std::cout << "\n" << stepsizes[i] << ", " << std::setprecision(8);

        const auto [ui, yi] = solve(stepsizes[i]);
    }

#endif // TIMING


#ifdef _DEBUG
    std::cout << "Error:\n\n";
    std::cout << "n, u, y\n";

    const auto [high_res_u, high_res_y] = solution;

    for (auto i = 0; i < IM_ARRAYSIZE(stepsizes); i++)
    {
        int s = stepsizes[i];

        const auto [high_res_u, high_res_y] = Example::getSolution(s);
        const auto [ui, yi] = solve(s);

        // std::cout << "Error Sum of Objective: " << std::setprecision(8) << get_err(yi[0], high_res_y[0], t0, t1) << "\n
        // std::cout << s << ", " << std::setprecision(8) << get_err(yi[0], high_res_y[0], t0, t1) << "\n";

        //std::cout << s << ", " << std::setprecision(8) << get_max_err(ui[0], high_res_u) << "\n";
        std::cout << s << ", " << std::setprecision(8) << get_max_err(ui[0], high_res_u) << ", " << std::setprecision(8) << get_max_err(yi[0], high_res_y) << "\n";
    }
#endif // _DEBUG
}

void Rendering::MainWindow::render()
{
    ImGui::Begin("Menu");

    ImGui::BeginGroup();
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Color::FOREGROUND);
    ImGui::PushStyleColor(ImGuiCol_Button, Color::FOREGROUND);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, Color::BACKGROUND);
    ImGui::PushStyleColor(ImGuiCol_Header, Color::FOREGROUND);

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(5.0f, 5.0f));

    // ===== Example Problems

    ImGui::Dummy(ImVec2(0.0f, 0.0f));
    ImGui::Text("Examples");
    ImGui::Dummy(ImVec2(0.0f, 2.0f));

    if (ImGui::Button("Hager Problem"))
    {
        //const size_t n = 10;

        Eigen::Matrix<std::function<double(double)>, 1, 1> F0, Fy, Fu;
       // auto y0 = Eigen::Matrix2d::Constant(1, 1, 1.0);

        F0 << [](double t) { return 0.0; };
        Fy << [](double t) { return 0.5; };
        Fu << [](double t) { return 1.0; };

        const auto [u,v] = Linear::solve_t(0, 1, RungeKutta::getTable(method), F0(0,0), Fy, Fu, steps, Eigen::MatrixXd::Constant(1, 1, 2.0), 0);
        const auto [r,s] = Example::getSolution(steps);
        
        control = u; state = v; show = true;
        x = r; y = s; 
        
        double maxErrorX = 0, maxErrorY = 0;

        //x = std::vector<double>(steps - 1, 0.0);
        //y = std::vector<double>(steps, 0.0);
        time = std::vector<double>(steps, 0);
        for (auto i = 0; i < steps; i++)
        {
            time[i] = (1.0 / steps) * i;

            maxErrorY = std::max(maxErrorY, abs(v[0][i] - s[i]));
            y[i] = abs(v[0][i] - s[i]);

            if (i < steps - 1)
            {
                maxErrorX = std::max(maxErrorX, abs(u[0][i] - r[i]));
                x[i] = abs(u[0][i] - r[i]);
            }
        }

        std::cout << "\nMax error: " << maxErrorX << " and " << maxErrorY << "\n\n";

        std::cout << "u: " << u[0][0] << "/" << r[0] << " -> " << u[0][u[0].size() - 1] << "/" << r[r.size() - 1] << "\n";
        std::cout << "y: " << v[0][0] << "/" << s[0] << " -> " << v[0][v[0].size() - 1] << "/" << s[steps - 1] << "\n";

        debug([=](int st) { return Linear::solve_t(0, 1, RungeKutta::getTable(method), F0(0, 0), Fy, Fu, st, Eigen::MatrixXd::Constant(1, 1, 2.0), 0); }, Example::getSolution(500));

//
//#ifdef _DEBUG
//
//        std::cout << "\n\n\nAUTOMATED TIMING TEST:\n\n";
//        // debug([=](int s) { return Linear::solve_t(0, 1, RungeKutta::getTable(method), F0(0, 0), Fy, Fu, s, Eigen::MatrixXd::Constant(1, 1, 2.0), 0); }, Example::getSolution(500));
//
//        print(4, steps, method);
//
//        for (auto j = 0; j < IM_ARRAYSIZE(stepsizes); j++)
//        {
//            double xMax = 0, yMax = 0;
//            int N = stepsizes[j];
//
//            const auto [sol_u, sol_y] = Example::getSolution(N);
//            const auto [ui, yi] = Linear::solve_t(0, 1, RungeKutta::getTable(method), F0(0, 0), Fy, Fu, N, Eigen::MatrixXd::Constant(1, 1, 2.0), 0);
//
//            // std::cout << "Error Sum of Objective: " << std::setprecision(8) << get_err(yi[0], high_res_y[0], t0, t1) << "\n
//            // std::cout << N << ", " << std::setprecision(8) << get_err(yi[0], s[0], t0, t1) << "\n";
//
//            for (auto i = 0; i < N - 1; i++)
//            {
//                /*
//                if (i > 0)
//                    xMax = std::max(xMax, ui[0][i] - sol_u[i]);
//                */
//                   
//                yMax = std::max(yMax, abs(yi[0][i] - sol_y[i]));
//            }
//
//            std::cout << N << ", " << std::setprecision(10) << yMax << "\n";
//        }
//
//#endif // _DEBUG
    }

    // ===== Runge-Kutta Method

    const char* items[] = { "Euler's", "Implicit Euler", "Heun's 2nd Order", "Classic RK4", "Ralston 4"};

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

    ImGui::InputInt("Steps", &steps, 1, 20);
    steps = std::min(std::max(steps, 3), 500);

    // ===== TODO: Calculate on button press

    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::Button("Solve");
    
    // ===== Print CSV

#ifdef _DEBUG
    // Print CSV for thesis graphs
    if (ImGui::Button("Print CSV to console"))
    {
        for (auto d = 0; d < state.size(); d++)
        {
            std::cout << "\n\n == Dimension " << (d + 1) << " =============================\n" << std::setw(10) << "t" << "," << std::setw(10) << "u" << "," << std::setw(10) << "y\n";
            std::cout << std::setprecision(8);
            for (auto i = 0; i < steps; i++)
                std::cout << std::setw(10) << ((t1 - t0) / (steps - 1)) * i << ", " << std::setw(10) << std::round(control[d][(int)std::min(i, steps - 2)] * 10000) / 10000 << ", " << std::setw(10) << std::round(state[d][i] * 10000) / 10000 << std::endl;
        }
    }
#endif // _DEBUG

    // ===== End GUI

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::EndGroup();

    ImGui::End();

    // ===== Render Plots

    if (show) {
        frame = PlotFrame("Example", 0, 1, control[0], state[0]);
        show = false;
    }

    if (control.size() > 0)
        frame.render();

    if (time.size() > 0)
    {
        ImGui::Begin("Result");

        if (ImPlot::BeginPlot("Runge-Kutta 2D"))
        {
            ImPlot::PushStyleColor(ImPlotCol_FrameBg, Color::BACKGROUND);
            ImPlot::PushStyleColor(ImPlotCol_Line, Color::DYNAMIC);
            ImPlot::PushStyleColor(ImPlotCol_Line, Color::CONTROL);
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
