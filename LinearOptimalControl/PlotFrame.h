#pragma once
#include <implot.h>
#include "Window.h"
#include "Color.h"
#include <vector>
#include <math.h>

namespace Rendering
{
	/// <summary>
	/// Plots the dynamic/control of the problem in a single dimension
	/// </summary>
	class PlotFrame
	{
    public:
        void drawPlot(const char* windowName)
        {
            ImGui::Begin(windowName);

            if (!ImPlot::BeginPlot(windowName))
                return;
            
            // Plot a line graph
            float data_control[100];
            float data_dynamic[100];
            for (int i = 0; i < 100; i++) {
                data_control[i] = sin(i * 0.1f);
                data_dynamic[i] = cos(i * 0.1f);
            }

            ImPlot::PushStyleColor(ImPlotCol_Line, Color::DYNAMIC);
            ImPlot::PlotLine("Dynamic", data_dynamic, 100);
            ImPlot::PopStyleColor();
            
            ImPlot::PushStyleColor(ImPlotCol_Line, Color::CONTROL);
            ImPlot::PlotLine("Control", data_control, 100);
            ImPlot::PopStyleColor();

            ImPlot::EndPlot();
            ImGui::End();
        }

        void render() {};

        PlotFrame(std::vector<double> control, std::vector<double> dynamic)
            : control(control), dynamic(dynamic)
        {

        }

        PlotFrame()
        {
            // For debugging
        }
    
    private:
        std::vector<double> control, dynamic;
	};

}
