#include <cmath>
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

TEST(PathOps, IntersectsConvexPolygonsWithExpectedArea) {
  const std::vector<manim_cpp::math::Vec2> a = {
      {0.0, 0.0}, {2.0, 0.0}, {2.0, 2.0}, {0.0, 2.0}};
  const std::vector<manim_cpp::math::Vec2> b = {
      {1.0, 1.0}, {3.0, 1.0}, {3.0, 3.0}, {1.0, 3.0}};

  const auto intersection = manim_cpp::math::intersect_convex_polygons(a, b);
  ASSERT_EQ(intersection.size(), static_cast<size_t>(4));
  EXPECT_NEAR(manim_cpp::math::polygon_area(intersection), 1.0, 1e-9);
}

TEST(PathOps, ReportsSignedAreaByVertexWinding) {
  const std::vector<manim_cpp::math::Vec2> ccw = {
      {0.0, 0.0}, {2.0, 0.0}, {0.0, 1.0}};
  const std::vector<manim_cpp::math::Vec2> cw = {
      {0.0, 0.0}, {0.0, 1.0}, {2.0, 0.0}};

  EXPECT_GT(manim_cpp::math::polygon_signed_area(ccw), 0.0);
  EXPECT_LT(manim_cpp::math::polygon_signed_area(cw), 0.0);
  EXPECT_DOUBLE_EQ(manim_cpp::math::polygon_area(ccw),
                   manim_cpp::math::polygon_area(cw));
}

TEST(PathOps, ComputesConvexUnionAreaUsingInclusionExclusion) {
  const std::vector<manim_cpp::math::Vec2> a = {
      {0.0, 0.0}, {2.0, 0.0}, {2.0, 2.0}, {0.0, 2.0}};
  const std::vector<manim_cpp::math::Vec2> b = {
      {1.0, 1.0}, {3.0, 1.0}, {3.0, 3.0}, {1.0, 3.0}};

  EXPECT_NEAR(manim_cpp::math::union_area_convex_polygons(a, b), 7.0, 1e-9);
}

TEST(PathOps, ComputesConvexDifferenceAreaByRemovingOverlap) {
  const std::vector<manim_cpp::math::Vec2> subject = {
      {0.0, 0.0}, {2.0, 0.0}, {2.0, 2.0}, {0.0, 2.0}};
  const std::vector<manim_cpp::math::Vec2> clip = {
      {1.0, 1.0}, {3.0, 1.0}, {3.0, 3.0}, {1.0, 3.0}};

  EXPECT_NEAR(manim_cpp::math::difference_area_convex_polygons(subject, clip), 3.0,
              1e-9);
}
