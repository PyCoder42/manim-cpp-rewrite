#pragma once

#include <cstdint>
#include <vector>

#include "manim_cpp/math/core.hpp"

namespace manim_cpp::math {

std::vector<uint32_t> triangulate_polygon_ear_clipping(const std::vector<Vec2>& polygon);

}  // namespace manim_cpp::math
