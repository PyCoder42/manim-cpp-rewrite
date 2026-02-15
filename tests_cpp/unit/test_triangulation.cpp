#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/math/triangulation.hpp"

namespace {

double polygon_area(const std::vector<manim_cpp::math::Vec2>& polygon) {
  if (polygon.size() < 3) {
    return 0.0;
  }

  double sum = 0.0;
  for (size_t i = 0; i < polygon.size(); ++i) {
    const auto& a = polygon[i];
    const auto& b = polygon[(i + 1) % polygon.size()];
    sum += (a[0] * b[1]) - (b[0] * a[1]);
  }
  return std::abs(sum) * 0.5;
}

double triangulation_area(const std::vector<manim_cpp::math::Vec2>& polygon,
                          const std::vector<uint32_t>& indices) {
  double sum = 0.0;
  for (size_t i = 0; i + 2 < indices.size(); i += 3) {
    const auto& a = polygon[indices[i]];
    const auto& b = polygon[indices[i + 1]];
    const auto& c = polygon[indices[i + 2]];
    const double twice = (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
    sum += std::abs(twice) * 0.5;
  }
  return sum;
}

}  // namespace

TEST(Triangulation, EarClippingReturnsEmptyForDegenerateInput) {
  const std::vector<manim_cpp::math::Vec2> line = {{0.0, 0.0}, {1.0, 0.0}};
  const auto indices = manim_cpp::math::triangulate_polygon_ear_clipping(line);
  EXPECT_TRUE(indices.empty());
}

TEST(Triangulation, EarClippingTriangulatesConvexPolygon) {
  const std::vector<manim_cpp::math::Vec2> pentagon = {
      {0.0, 0.0}, {2.0, 0.0}, {3.0, 1.0}, {1.5, 2.0}, {0.0, 1.0}};

  const auto indices = manim_cpp::math::triangulate_polygon_ear_clipping(pentagon);
  ASSERT_EQ(indices.size(), static_cast<size_t>((pentagon.size() - 2) * 3));

  for (const auto index : indices) {
    EXPECT_LT(index, pentagon.size());
  }
}

TEST(Triangulation, EarClippingPreservesAreaForConcavePolygon) {
  const std::vector<manim_cpp::math::Vec2> concave = {
      {0.0, 0.0}, {3.0, 0.0}, {3.0, 1.0}, {1.5, 0.5}, {0.0, 1.0}};

  const auto indices = manim_cpp::math::triangulate_polygon_ear_clipping(concave);
  ASSERT_EQ(indices.size(), static_cast<size_t>((concave.size() - 2) * 3));

  const double source_area = polygon_area(concave);
  const double triangles_area = triangulation_area(concave, indices);
  EXPECT_NEAR(triangles_area, source_area, 1e-9);
}
