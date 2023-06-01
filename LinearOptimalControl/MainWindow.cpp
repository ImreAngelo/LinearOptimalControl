#include "MainWindow.h"
#include "LinearProgram.h"
#include "RungeKutta.h"
#include "Color.h"
#include <imgui.h>

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

double integrate(std::vector<double> f, double t0, double t1)
{
    const double dt = (t1 - t0) / f.size();
    double sum = 0;

    for (auto i = 0; i < f.size() - 1; i++)
        sum += ((dt) / 2) * (f[i] + f[i + 1]);

    return sum;
}

double get_err(std::vector<double> approx, double solution, double t0 = 0, double t1 = 1)
{
    const double a = integrate(approx, t0, t1);
    // std::cout << "approximation: " << a << "\nsolution: " << b << "\n";
    return std::abs(a - solution);
}

double get_err(std::vector<double> approx, std::vector<double> solution, double t0 = 0, double t1 = 1)
{
    const double b = integrate(solution, t0, t1);
    return get_err(approx, b, t0, t1);
}

//constexpr int stepsizes[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 120, 140, 160, 180, 200, 250 };
constexpr int stepsizes[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 120, 140, 160, 180, 200, 250, 300, 350, 400, 450, 500 };

void debug(std::function<Linear::Solution(size_t, int)> solve, int method, double t0 = 0, double t1 = 1, double solution = 0)
{
    std::cout << "\n\nSOLVING HIGH RESOLUTION (IGNORE)\n\n";
    const auto [high_res_u, high_res_y] = solve((solution) ? 1 : 500, 3);

#ifdef TIMING

    std::cout << "\n\nn, int, param, cplex";

    for (auto i = 0; i < IM_ARRAYSIZE(stepsizes); i++)
    {
        std::cout << "\n" << stepsizes[i] << ", " << std::setprecision(8);

        const auto [ui, yi] = solve(stepsizes[i], method);
    }

#endif // TIMING
#ifdef _DEBUG

    //std::cout << "\n\nn, param, cplex\n";
    std::cout << "\n\nn, y\n";

    for (auto i = 0; i < IM_ARRAYSIZE(stepsizes); i++)
    {
        const auto [ui, yi] = solve(stepsizes[i], method);

        std::cout << stepsizes[i] << ", " << std::setprecision(8) << ((solution) ? get_err(yi[0], solution, t0, t1) : get_err(yi[0], high_res_y[0], t0, t1)) << "\n";
        // std::cout << "Objective: " << std::setprecision(8) << integrate(yi[1], t0, t1) << " / " << solution << "\n";
        // std::cout << "Error Sum of Objective: " << std::setprecision(8) << get_err(yi[0], high_res_y[0], t0, t1) << "\n";
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

    if (ImGui::Button("Problem 1")) {
        t0 = 0;
        t1 = 2;

        print(1, steps, method);

        const auto ph = Eigen::MatrixXd::Constant(1, 1, 1.0);
        const auto F0 = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return  0.0; });
        const auto Fu = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return -1.0; });

        const auto [ ut, yt ] = Linear::solve_t(t0, t1, RungeKutta::getTable(method), F0(0,0), F0, Fu, steps + 1, ph);
        control = ut; state = yt; show = true;

        debug([=](size_t s, int m) { return Linear::solve_t(t0, t1, RungeKutta::getTable(m), F0(0, 0), F0, Fu, s, ph); }, method, t0, t1, 0.5);
    }

    ImGui::SameLine();

    if (ImGui::Button("Problem 2")) {
        t0 = 0;
        t1 = 3;

        print(2, steps, method);

        const auto ph = Eigen::MatrixXd::Constant(1, 1, 1.0);
        const auto Fy = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return .7; });
        const auto Fu = Eigen::Matrix<std::function<double(double)>, 1, 1>::Constant([](double t) { return -1; });
        const auto Fc = [](double t) { return .1*t; };

        const auto [ut, yt] = Linear::solve_t(t0, t1, RungeKutta::getTable(method), Fc, Fy, Fu, steps, ph);
        control = ut; state = yt; show = true;

        debug([=](size_t s, int m) { return Linear::solve_t(t0, t1, RungeKutta::getTable(m), Fc, Fy, Fu, s, ph); }, method, t0, t1);
    }

    ImGui::SameLine();

    if (ImGui::Button("Problem 3")) {
        t0 = 0; t1 = 1;

        const auto Fy = Eigen::Matrix<std::function<double(double)>, 2, 2>::Constant([](double t) { return 0; });
        const auto Fc = [](double t) { return 0; };

        Eigen::Matrix<std::function<double(double)>, 2, 2> Fu;
        Eigen::Matrix<double, 1, 2> phi;

        Fu << [](double t) { return -1.0; }, [](double t) { return 0.0; },
              [](double t) { return 0.0; }, [](double t) { return 1.0; };

        phi << 0.0, -1.0;

        print(3, steps, method);

        auto [ ut, yt ] = Linear::solve_t(t0, t1, RungeKutta::getTable(method), Fc, Fy, Fu, steps, phi, 1);
        control = ut; state = yt; show = true;

        x = ut[1];
        y = yt[1];

        time = std::vector<double>(x.size(), 0.0);
        for (auto i = 0; i < x.size(); i++)
            time[i] = ((t1 - t0) / steps) * i;

        const auto [high_res_u, high_res_y] = Linear::solve_t(t0, t1, RungeKutta::getTable(method), Fc, Fy, Fu, 249, phi, 1);
        const auto err = get_err(yt[1], high_res_y[1], t0, t1);
        const auto sol = integrate(high_res_y[1], t0, t1);

        // std::cout << "Error Sum of Objective: " << err << "\n\n";

        debug([=](size_t s, int m) { return Linear::solve_t(t0, t1, RungeKutta::getTable(m), Fc, Fy, Fu, s, phi, 1); }, method, t0, t1, sol);
    }

    // ===== Runge-Kutta Method

    const char* items[] = { "Euler's", "Implicit Euler", "Heun's 2nd Order", "Classic RK4", "Ralston's 4th Order"};

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

//#ifdef _DEBUG
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
//#endif // _DEBUG

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
        frame = PlotFrame("Window 1", t0, t1, control[0], state[0]);
        show = false;
    }

    if (control.size() > 0)
        frame.render();

    if (time.size() > 0)
    {
        ImGui::Begin("Result");

        if (ImPlot::BeginPlot("Window 2"))
        {
            ImPlot::PushStyleColor(ImPlotCol_FrameBg, Color::BACKGROUND);
            ImPlot::PushStyleColor(ImPlotCol_Line, Color::DYNAMIC);
            ImPlot::PushStyleColor(ImPlotCol_Line, Color::CONTROL);
            ImPlot::PlotLine("u(t)", &time[0], &x[0], time.size());
            ImPlot::PopStyleColor();
            ImPlot::PlotLine("y(t)", &time[0], &y[0], time.size());
            ImPlot::PopStyleColor();
            ImPlot::PopStyleColor();
            ImPlot::EndPlot();
        }

        ImGui::End();
    }
}
