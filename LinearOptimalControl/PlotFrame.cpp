#include "PlotFrame.h"

template <typename T>
std::vector<T> linspace(T a, T b, size_t N) {
    T h = (b - a) / static_cast<T>(N - 1);
    std::vector<T> xs(N);
    typename std::vector<T>::iterator x;
    T val;
    for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
        *x = val;
    return xs;
}

void Rendering::PlotFrame::render()
{
    ImGui::Begin(windowName.c_str());

    if (ImPlot::BeginPlot("Control"))
    {
        ImPlot::PushStyleColor(ImPlotCol_FrameBg, Color::FOREGROUND);
        ImPlot::SetNextLineStyle(Color::CONTROL, 1.5);
        ImPlot::PlotLine("u(t)", &linspace(t0, t1, control.size())[0], &control[0], control.size());
        ImPlot::PopStyleColor();
    //    ImPlot::EndPlot();
    //}

    //if (ImPlot::BeginPlot("Objective"))
    //{
        ImPlot::PushStyleColor(ImPlotCol_FrameBg, Color::FOREGROUND);
        ImPlot::SetNextLineStyle(Color::DYNAMIC, 1.5);
        ImPlot::PlotLine("y(t)", &linspace(t0, t1, dynamic.size())[0], &dynamic[0], dynamic.size());
        ImPlot::PopStyleColor();
        ImPlot::EndPlot();
    }

    ImGui::End();
}
