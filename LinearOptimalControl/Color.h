#pragma once
#include <imgui.h>

namespace Color
{
	constexpr ImVec4 color(float r, float g, float b) {
		return { r/255, g/255, b/255, 1.0f };
	}

	constexpr ImVec4 BACKGROUND = color(26, 26, 26);
	constexpr ImVec4 FOREGROUND = color(0, 26, 26);
	constexpr ImVec4 DYNAMIC = color(250, 10, 31);
	constexpr ImVec4 CONTROL = color(10, 250, 31);
}
