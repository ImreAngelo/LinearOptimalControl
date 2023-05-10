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
		//LinearProgram lp{};
		PlotFrame frame{"d"};
		bool show = false;
	};
}

