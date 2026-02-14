#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/math/path_ops.hpp"

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
