#include "manim_cpp/math/graph_layout.hpp"

#include <cmath>
#include <vector>

namespace manim_cpp::math {

std::vector<Vec2> circular_layout(const std::size_t node_count, const double radius,
                                  const double phase) {
  std::vector<Vec2> points;
  if (node_count == 0) {
    return points;
  }

  points.reserve(node_count);
  const double delta = (2.0 * kPi) / static_cast<double>(node_count);
  for (std::size_t index = 0; index < node_count; ++index) {
    const double angle = phase + (delta * static_cast<double>(index));
    points.push_back(Vec2{radius * std::cos(angle), radius * std::sin(angle)});
  }
  return points;
}

}  // namespace manim_cpp::math
