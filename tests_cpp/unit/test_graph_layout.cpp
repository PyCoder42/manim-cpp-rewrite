#include <cmath>

#include <gtest/gtest.h>

#include "manim_cpp/math/graph_layout.hpp"

TEST(GraphLayout, CircularLayoutHandlesEmptyInput) {
  const auto points = manim_cpp::math::circular_layout(0, 2.0);
  EXPECT_TRUE(points.empty());
}

TEST(GraphLayout, CircularLayoutProducesCardinalPointsForFourNodes) {
  const auto points = manim_cpp::math::circular_layout(4, 2.0);
  ASSERT_EQ(points.size(), static_cast<size_t>(4));

  EXPECT_NEAR(points[0][0], 2.0, 1e-9);
  EXPECT_NEAR(points[0][1], 0.0, 1e-9);
  EXPECT_NEAR(points[1][0], 0.0, 1e-9);
  EXPECT_NEAR(points[1][1], 2.0, 1e-9);
  EXPECT_NEAR(points[2][0], -2.0, 1e-9);
  EXPECT_NEAR(points[2][1], 0.0, 1e-9);
  EXPECT_NEAR(points[3][0], 0.0, 1e-9);
  EXPECT_NEAR(points[3][1], -2.0, 1e-9);
}

TEST(GraphLayout, CircularLayoutRespectsPhaseOffset) {
  const auto points = manim_cpp::math::circular_layout(1, 3.0, manim_cpp::math::kPi / 2.0);
  ASSERT_EQ(points.size(), static_cast<size_t>(1));
  EXPECT_NEAR(points[0][0], 0.0, 1e-9);
  EXPECT_NEAR(points[0][1], 3.0, 1e-9);
}
