#pragma once
#include "Window.h"
#include <implot.h>
#include "Color.h"
#include <vector>
#include <math.h>

namespace Rendering
{
	/// <summary>
	/// Plots the dynamic/control of the problem in a single dimension
	/// </summary>
	class PlotFrame : public Window
	{
    public:
        virtual void render() override;

        PlotFrame(const char* title, std::vector<double> control, std::vector<double> dynamic)
            : windowName(title), control(control), dynamic(dynamic) { }

        PlotFrame(const char* title)
            : windowName(title)
        {
            // For debugging
            control.reserve(100);
            dynamic.reserve(100);

            for (int i = 0; i < 100; i++) {
                control.emplace_back(sin(i * .1f));
                dynamic.emplace_back(.9f*cos(i * .075f));
            }
        }
    
    private:
        std::vector<double> control, dynamic;
        const char* windowName;
	};

}
