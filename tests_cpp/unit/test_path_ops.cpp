#include <cmath>
#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/math/path_ops.hpp"

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

}  // namespace

TEST(PathOps, DetectsSegmentIntersection) {
  const manim_cpp::math::Vec2 a0 = {0.0, 0.0};
  const manim_cpp::math::Vec2 a1 = {2.0, 2.0};
  const manim_cpp::math::Vec2 b0 = {0.0, 2.0};
  const manim_cpp::math::Vec2 b1 = {2.0, 0.0};

  EXPECT_TRUE(manim_cpp::math::segments_intersect(a0, a1, b0, b1));
}

TEST(PathOps, DetectsNonIntersectingSegments) {
  const manim_cpp::math::Vec2 a0 = {0.0, 0.0};
  const manim_cpp::math::Vec2 a1 = {1.0, 0.0};
  const manim_cpp::math::Vec2 b0 = {2.0, 0.0};
  const manim_cpp::math::Vec2 b1 = {3.0, 0.0};

  EXPECT_FALSE(manim_cpp::math::segments_intersect(a0, a1, b0, b1));
}

TEST(PathOps, PointInPolygonReportsInsideAndOutside) {
  const std::vector<manim_cpp::math::Vec2> square = {
      {0.0, 0.0}, {2.0, 0.0}, {2.0, 2.0}, {0.0, 2.0}};

  EXPECT_TRUE(manim_cpp::math::point_in_polygon(square, {1.0, 1.0}));
  EXPECT_FALSE(manim_cpp::math::point_in_polygon(square, {3.0, 1.0}));
}

TEST(PathOps, DetectsPolygonSelfIntersection) {
  const std::vector<manim_cpp::math::Vec2> bow_tie = {
      {0.0, 0.0}, {2.0, 2.0}, {0.0, 2.0}, {2.0, 0.0}};

  EXPECT_TRUE(manim_cpp::math::has_self_intersections(bow_tie));
}

TEST(PathOps, IntersectsConvexPolygonsWithExpectedArea) {
  const std::vector<manim_cpp::math::Vec2> a = {
      {0.0, 0.0}, {2.0, 0.0}, {2.0, 2.0}, {0.0, 2.0}};
  const std::vector<manim_cpp::math::Vec2> b = {
      {1.0, 1.0}, {3.0, 1.0}, {3.0, 3.0}, {1.0, 3.0}};

  const auto intersection = manim_cpp::math::intersect_convex_polygons(a, b);
  ASSERT_EQ(intersection.size(), static_cast<size_t>(4));
  EXPECT_NEAR(polygon_area(intersection), 1.0, 1e-9);
}
