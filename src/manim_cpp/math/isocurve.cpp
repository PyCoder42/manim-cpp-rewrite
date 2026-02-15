#include "manim_cpp/math/isocurve.hpp"

#include <cmath>
#include <cstddef>
#include <vector>

namespace manim_cpp::math {
namespace {

bool crosses_iso(const double a, const double b, const double iso) {
  const bool a_inside = a >= iso;
  const bool b_inside = b >= iso;
  return a_inside != b_inside;
}

Vec2 lerp_point(const Vec2& a, const Vec2& b, const double va, const double vb,
                const double iso) {
  const double denominator = vb - va;
  if (std::abs(denominator) < 1e-12) {
    return a;
  }
  const double t = (iso - va) / denominator;
  return Vec2{a[0] + t * (b[0] - a[0]), a[1] + t * (b[1] - a[1])};
}

}  // namespace

std::vector<Segment2D> extract_isocurve_segments(
    const std::vector<std::vector<double>>& scalar_field, const double iso_value,
    const double cell_size) {
  std::vector<Segment2D> segments;
  if (scalar_field.size() < 2 || scalar_field.front().size() < 2) {
    return segments;
  }

  const std::size_t width = scalar_field.front().size();
  for (const auto& row : scalar_field) {
    if (row.size() != width) {
      return {};
    }
  }

  for (std::size_t y = 0; y + 1 < scalar_field.size(); ++y) {
    for (std::size_t x = 0; x + 1 < width; ++x) {
      const double v0 = scalar_field[y][x];
      const double v1 = scalar_field[y][x + 1];
      const double v2 = scalar_field[y + 1][x + 1];
      const double v3 = scalar_field[y + 1][x];

      const Vec2 p0 = {static_cast<double>(x) * cell_size,
                       static_cast<double>(y) * cell_size};
      const Vec2 p1 = {static_cast<double>(x + 1) * cell_size,
                       static_cast<double>(y) * cell_size};
      const Vec2 p2 = {static_cast<double>(x + 1) * cell_size,
                       static_cast<double>(y + 1) * cell_size};
      const Vec2 p3 = {static_cast<double>(x) * cell_size,
                       static_cast<double>(y + 1) * cell_size};

      std::vector<Vec2> intersections;
      intersections.reserve(4);
      if (crosses_iso(v0, v1, iso_value)) {
        intersections.push_back(lerp_point(p0, p1, v0, v1, iso_value));
      }
      if (crosses_iso(v1, v2, iso_value)) {
        intersections.push_back(lerp_point(p1, p2, v1, v2, iso_value));
      }
      if (crosses_iso(v2, v3, iso_value)) {
        intersections.push_back(lerp_point(p2, p3, v2, v3, iso_value));
      }
      if (crosses_iso(v3, v0, iso_value)) {
        intersections.push_back(lerp_point(p3, p0, v3, v0, iso_value));
      }

      if (intersections.size() == 2) {
        segments.push_back(Segment2D{.start = intersections[0], .end = intersections[1]});
      } else if (intersections.size() == 4) {
        // Deterministic pairing for ambiguous marching-squares cases.
        segments.push_back(Segment2D{.start = intersections[0], .end = intersections[1]});
        segments.push_back(Segment2D{.start = intersections[2], .end = intersections[3]});
      }
    }
  }

  return segments;
}

}  // namespace manim_cpp::math
