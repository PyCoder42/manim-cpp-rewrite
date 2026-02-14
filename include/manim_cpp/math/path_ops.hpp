#pragma once

#include <vector>

#include "manim_cpp/math/core.hpp"

namespace manim_cpp::math {

bool segments_intersect(const Vec2& a0, const Vec2& a1, const Vec2& b0, const Vec2& b1);

bool point_in_polygon(const std::vector<Vec2>& polygon, const Vec2& point);

bool has_self_intersections(const std::vector<Vec2>& polygon);

}  // namespace manim_cpp::math
