#pragma once
#include <imgui.h>

namespace Color
{
	constexpr ImVec4 colorFromRGB(float r, float g, float b) {
		return { r/255, g/255, b/255, 1.0f };
	}

	constexpr ImVec4 BACKGROUND = colorFromRGB(250, 10, 10);
	constexpr ImVec4 FOREGROUND = colorFromRGB(26, 26, 26);
	constexpr ImVec4 DYNAMIC = colorFromRGB(250, 10, 31);
	constexpr ImVec4 CONTROL = colorFromRGB(10, 250, 31);
}
