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

	private:
		std::vector<double> time, x, y;
	};
}

