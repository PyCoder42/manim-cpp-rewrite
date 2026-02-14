#pragma once

#include <cstddef>
#include <vector>

#include "manim_cpp/math/core.hpp"

namespace manim_cpp::math {

std::vector<Vec2> circular_layout(std::size_t node_count, double radius, double phase = 0.0);

}  // namespace manim_cpp::math
