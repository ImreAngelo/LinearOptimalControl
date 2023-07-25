#pragma once
#include "Window.h"
#include "PlotFrame.h"

namespace Rendering
{
	class MainWindow : public Window
	{
		// Inherited via Window
		virtual void render() override;

	private:
		PlotFrame frame{"d"};
		bool show = false;
		int method = 0;
		int steps = 200;
		double t0 = 0, t1 = 1;

	private:
		std::vector<double> time, x, y;
		std::vector<std::vector<double>> control, state;
		
		void example_one();
		void example_two();
		void example_three();
	};
}

