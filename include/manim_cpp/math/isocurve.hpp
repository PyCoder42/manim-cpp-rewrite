#pragma once

#include <vector>

#include "manim_cpp/math/core.hpp"

namespace manim_cpp::math {

struct Segment2D {
  Vec2 start;
  Vec2 end;
};

std::vector<Segment2D> extract_isocurve_segments(
    const std::vector<std::vector<double>>& scalar_field,
    double iso_value,
    double cell_size = 1.0);

}  // namespace manim_cpp::math
