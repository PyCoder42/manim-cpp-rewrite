#include "manim_cpp/math/core.hpp"

#include <cmath>

namespace manim_cpp::math {

uint64_t choose(uint64_t n, uint64_t k) {
  if (k > n) {
    return 0;
  }

  if (k == 0 || k == n) {
    return 1;
  }

  if (k > n - k) {
    k = n - k;
  }

  uint64_t result = 1;
  for (uint64_t i = 1; i <= k; ++i) {
    result = (result * (n - k + i)) / i;
  }
  return result;
}

Vec3 rotate_z(const Vec3& point, double radians) {
  const double c = std::cos(radians);
  const double s = std::sin(radians);
  return Vec3{c * point[0] - s * point[1], s * point[0] + c * point[1], point[2]};
}

std::vector<uint32_t> triangulate_polygon_fan(const std::vector<Vec2>& polygon) {
  std::vector<uint32_t> indices;
  if (polygon.size() < 3) {
    return indices;
  }

  indices.reserve((polygon.size() - 2) * 3);
  for (uint32_t i = 1; i + 1 < polygon.size(); ++i) {
    indices.push_back(0);
    indices.push_back(i);
    indices.push_back(i + 1);
  }
  return indices;
}

}  // namespace manim_cpp::math
