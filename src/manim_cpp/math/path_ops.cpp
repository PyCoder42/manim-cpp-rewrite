#include "manim_cpp/math/path_ops.hpp"

#include <algorithm>
#include <cmath>

namespace manim_cpp::math {
namespace {

constexpr double kEpsilon = 1e-12;

double orientation(const Vec2& a, const Vec2& b, const Vec2& c) {
  return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
}

bool on_segment(const Vec2& a, const Vec2& b, const Vec2& point) {
  if (std::abs(orientation(a, b, point)) > kEpsilon) {
    return false;
  }

  return point[0] >= std::min(a[0], b[0]) - kEpsilon &&
         point[0] <= std::max(a[0], b[0]) + kEpsilon &&
         point[1] >= std::min(a[1], b[1]) - kEpsilon &&
         point[1] <= std::max(a[1], b[1]) + kEpsilon;
}

}  // namespace

bool segments_intersect(const Vec2& a0, const Vec2& a1, const Vec2& b0, const Vec2& b1) {
  const double o1 = orientation(a0, a1, b0);
  const double o2 = orientation(a0, a1, b1);
  const double o3 = orientation(b0, b1, a0);
  const double o4 = orientation(b0, b1, a1);

  const bool proper_intersection =
      ((o1 > kEpsilon && o2 < -kEpsilon) || (o1 < -kEpsilon && o2 > kEpsilon)) &&
      ((o3 > kEpsilon && o4 < -kEpsilon) || (o3 < -kEpsilon && o4 > kEpsilon));

  if (proper_intersection) {
    return true;
  }

  if (on_segment(a0, a1, b0) || on_segment(a0, a1, b1) || on_segment(b0, b1, a0) ||
      on_segment(b0, b1, a1)) {
    return true;
  }

  return false;
}

bool point_in_polygon(const std::vector<Vec2>& polygon, const Vec2& point) {
  if (polygon.size() < 3) {
    return false;
  }

  bool inside = false;
  for (size_t i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
    const auto& pi = polygon[i];
    const auto& pj = polygon[j];

    if (on_segment(pj, pi, point)) {
      return true;
    }

    const bool crosses_vertical_span = ((pi[1] > point[1]) != (pj[1] > point[1]));
    if (!crosses_vertical_span) {
      continue;
    }

    const double x_on_edge = ((pj[0] - pi[0]) * (point[1] - pi[1])) / (pj[1] - pi[1]) + pi[0];
    if (point[0] < x_on_edge) {
      inside = !inside;
    }
  }

  return inside;
}

bool has_self_intersections(const std::vector<Vec2>& polygon) {
  if (polygon.size() < 4) {
    return false;
  }

  const size_t edge_count = polygon.size();
  for (size_t i = 0; i < edge_count; ++i) {
    const auto& a0 = polygon[i];
    const auto& a1 = polygon[(i + 1) % edge_count];

    for (size_t j = i + 1; j < edge_count; ++j) {
      if (i == j || (i + 1) % edge_count == j || i == (j + 1) % edge_count) {
        continue;
      }

      const auto& b0 = polygon[j];
      const auto& b1 = polygon[(j + 1) % edge_count];
      if (segments_intersect(a0, a1, b0, b1)) {
        return true;
      }
    }
  }

  return false;
}

}  // namespace manim_cpp::math
