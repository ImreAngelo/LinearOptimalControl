#include "PlotFrame.h"

void Rendering::PlotFrame::render()
{
    ImGui::Begin(windowName);

    if (!ImPlot::BeginPlot(windowName))
        return;

    ImPlot::PushStyleColor(ImPlotCol_Line, Color::CONTROL);
    ImPlot::PlotLine("Control", &control[0], control.size());
    ImPlot::PopStyleColor();

    ImPlot::PushStyleColor(ImPlotCol_Line, Color::DYNAMIC);
    ImPlot::PlotLine("Dynamic", &dynamic[0], dynamic.size());
    ImPlot::PopStyleColor();

    ImPlot::EndPlot();
    ImGui::End();
}
