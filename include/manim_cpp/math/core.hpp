#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace manim_cpp::math {

inline constexpr double kPi = 3.14159265358979323846;

using Vec2 = std::array<double, 2>;
using Vec3 = std::array<double, 3>;

uint64_t choose(uint64_t n, uint64_t k);
uint64_t factorial(uint64_t n);
uint64_t permutations(uint64_t n, uint64_t k);

Vec3 rotate_z(const Vec3& point, double radians);

std::vector<uint32_t> triangulate_polygon_fan(const std::vector<Vec2>& polygon);

}  // namespace manim_cpp::math
