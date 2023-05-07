#pragma once
#include <implot.h>
#include <math.h>
#include "Window.h"

#define DIVIDE(v) v/((float)255)
#define FROM_RGB(r,g,b) DIVIDE(r),DIVIDE(g),DIVIDE(b),1
#define COLOR(name,r,g,b) ImVec4 name{FROM_RGB(r,g,b)}

namespace Rendering
{
	/// <summary>
	/// Plots the dynamic/control of the problem in a single dimension
	/// </summary>
	class PlotFrame
	{
    public:
        struct {
            COLOR(bk, 26, 26, 26);
            COLOR(fg, 0, 10, 21);
            COLOR(dynamic, 250, 10, 31);
            COLOR(control, 10, 250, 31);
        } colors;

    public:
        void drawPlot(const char* windowName)
        {
            if (ImPlot::BeginPlot(windowName))
            {
                // Plot a line graph
                float data_control[100];
                float data_dynamic[100];
                for (int i = 0; i < 100; i++) {
                    data_control[i] = sin(i * 0.1f);
                    data_dynamic[i] = cos(i * 0.1f);
                }

                ImPlot::PushStyleColor(ImPlotCol_Line, colors.dynamic);
                ImPlot::PlotLine("Dynamic", data_dynamic, 100);
                ImPlot::PopStyleColor();
    
                /*ImGui::PushStyleColor(ImGuiCol_FrameBg, colors.fg);
                ImGui::PushStyleColor(ImGuiCol_Text, colors.control);
                */
                ImPlot::PushStyleColor(ImPlotCol_Line, colors.control);
                ImPlot::PlotLine("Control", data_control, 100);
                ImPlot::PopStyleColor();
                /*ImGui::PopStyleColor();
                ImGui::PopStyleColor();*/

                ImPlot::EndPlot();
            }
        }
	};
}
