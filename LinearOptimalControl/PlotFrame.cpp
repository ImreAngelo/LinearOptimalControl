#include "PlotFrame.h"

void Rendering::PlotFrame::render()
{
    ImGui::Begin(windowName);

    if (ImPlot::BeginPlot("Control"))
    {
        ImPlot::PushStyleColor(ImPlotCol_Line, Color::CONTROL);
        ImPlot::PlotLine("u(t)", &control[0], control.size());
        ImPlot::PopStyleColor();
        ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Dynamic"))
    {
        ImPlot::PushStyleColor(ImPlotCol_Line, Color::DYNAMIC);
        ImPlot::PlotLine("y(t)", &dynamic[0], dynamic.size());
        ImPlot::PopStyleColor();
        ImPlot::EndPlot();
    }

    ImGui::End();
}
