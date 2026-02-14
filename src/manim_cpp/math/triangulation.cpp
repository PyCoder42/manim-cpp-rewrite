#include "manim_cpp/math/triangulation.hpp"

#include <cmath>
#include <cstddef>
#include <vector>

namespace manim_cpp::math {
namespace {

constexpr double kEpsilon = 1e-12;

double cross(const Vec2& a, const Vec2& b, const Vec2& c) {
  return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
}

double signed_area(const std::vector<Vec2>& polygon) {
  double sum = 0.0;
  for (size_t i = 0; i < polygon.size(); ++i) {
    const auto& a = polygon[i];
    const auto& b = polygon[(i + 1) % polygon.size()];
    sum += (a[0] * b[1]) - (b[0] * a[1]);
  }
  return 0.5 * sum;
}

bool point_in_triangle(const Vec2& point, const Vec2& a, const Vec2& b, const Vec2& c) {
  const double c1 = cross(a, b, point);
  const double c2 = cross(b, c, point);
  const double c3 = cross(c, a, point);

  const bool has_negative = (c1 < -kEpsilon) || (c2 < -kEpsilon) || (c3 < -kEpsilon);
  const bool has_positive = (c1 > kEpsilon) || (c2 > kEpsilon) || (c3 > kEpsilon);
  return !(has_negative && has_positive);
}

}  // namespace

std::vector<uint32_t> triangulate_polygon_ear_clipping(const std::vector<Vec2>& polygon) {
  std::vector<uint32_t> triangles;
  if (polygon.size() < 3) {
    return triangles;
  }

  std::vector<uint32_t> vertex_indices(polygon.size());
  for (uint32_t i = 0; i < polygon.size(); ++i) {
    vertex_indices[i] = i;
  }

  const bool ccw = signed_area(polygon) > 0.0;

  while (vertex_indices.size() > 3) {
    bool ear_found = false;

    for (size_t i = 0; i < vertex_indices.size(); ++i) {
      const uint32_t prev = vertex_indices[(i + vertex_indices.size() - 1) % vertex_indices.size()];
      const uint32_t curr = vertex_indices[i];
      const uint32_t next = vertex_indices[(i + 1) % vertex_indices.size()];

      const auto& a = polygon[prev];
      const auto& b = polygon[curr];
      const auto& c = polygon[next];

      const double turn = cross(a, b, c);
      if ((ccw && turn <= kEpsilon) || (!ccw && turn >= -kEpsilon)) {
        continue;
      }

      bool contains_other_point = false;
      for (const auto candidate : vertex_indices) {
        if (candidate == prev || candidate == curr || candidate == next) {
          continue;
        }
        if (point_in_triangle(polygon[candidate], a, b, c)) {
          contains_other_point = true;
          break;
        }
      }

      if (contains_other_point) {
        continue;
      }

      triangles.push_back(prev);
      triangles.push_back(curr);
      triangles.push_back(next);
      vertex_indices.erase(vertex_indices.begin() + static_cast<std::ptrdiff_t>(i));
      ear_found = true;
      break;
    }

    if (!ear_found) {
      return {};
    }
  }

  triangles.push_back(vertex_indices[0]);
  triangles.push_back(vertex_indices[1]);
  triangles.push_back(vertex_indices[2]);
  return triangles;
}

}  // namespace manim_cpp::math
