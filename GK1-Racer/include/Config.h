#pragma once

#include <cstdint>
#include <glm/glm.hpp>

static inline consteval glm::vec4 make_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
{
	glm::vec4 v; v.x = r / 255.0f; v.y = g / 255.0f; v.z = b / 255.0f; v.w = a / 255.0f; return v;
}

namespace Config 
{
	static constexpr int WINDOW_WIDTH = 1600;
	static constexpr int WINDOW_HEIGHT = 900;
	static constexpr glm::vec4 CLEAR_COLOR = make_color(100, 149, 237);
}