#include <array>
#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/math/core.hpp"

TEST(MathCore, BinomialChooseComputesExpectedValues) {
  EXPECT_EQ(manim_cpp::math::choose(5, 2), 10ULL);
  EXPECT_EQ(manim_cpp::math::choose(10, 0), 1ULL);
  EXPECT_EQ(manim_cpp::math::choose(10, 10), 1ULL);
}

TEST(MathCore, CombinatoricsHelpersComputeExpectedValues) {
  EXPECT_EQ(manim_cpp::math::factorial(0), 1ULL);
  EXPECT_EQ(manim_cpp::math::factorial(5), 120ULL);
  EXPECT_EQ(manim_cpp::math::permutations(5, 2), 20ULL);
  EXPECT_EQ(manim_cpp::math::permutations(4, 4), 24ULL);
}

TEST(MathCore, RotateZAroundNinetyDegrees) {
  const manim_cpp::math::Vec3 point{1.0, 0.0, 0.0};
  const auto rotated = manim_cpp::math::rotate_z(point, manim_cpp::math::kPi / 2.0);
  EXPECT_NEAR(rotated[0], 0.0, 1e-9);
  EXPECT_NEAR(rotated[1], 1.0, 1e-9);
  EXPECT_NEAR(rotated[2], 0.0, 1e-9);
}

TEST(MathCore, TriangulateSquareWithFanStrategy) {
  const std::vector<manim_cpp::math::Vec2> polygon = {
      {0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}};

  const auto indices = manim_cpp::math::triangulate_polygon_fan(polygon);
  ASSERT_EQ(indices.size(), static_cast<size_t>(6));

  const std::vector<uint32_t> expected = {0, 1, 2, 0, 2, 3};
  EXPECT_EQ(indices, expected);
}
