#pragma once
#include <imgui.h>

namespace Color
{
	constexpr ImVec4 rgb(float r, float g, float b) {
		return { r/255, g/255, b/255, 1.0f };
	}

	constexpr ImVec4 BACKGROUND = rgb(18, 18, 18);
	constexpr ImVec4 FOREGROUND = rgb(26, 26, 26);
	constexpr ImVec4 DYNAMIC = rgb(255, 96, 0); // rgb(250, 10, 31);
	constexpr ImVec4 CONTROL = rgb(143, 67, 238); // rgb(10, 250, 31);
}
