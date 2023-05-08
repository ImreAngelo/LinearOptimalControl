#pragma once
#include "Window.h"
#include "LinearProgram.h"
#include "PlotFrame.h"

namespace Rendering
{
	class MainWindow : public Window
	{
		// Inherited via Window
		virtual void render() override;

	private:
		LinearProgram lp{};
		PlotFrame frame{ "DEBUG" };
		bool show = false;
	};
}

