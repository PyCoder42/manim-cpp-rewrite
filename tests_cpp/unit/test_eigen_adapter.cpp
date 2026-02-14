#include <gtest/gtest.h>

#include "manim_cpp/math/eigen_adapter.hpp"

TEST(EigenAdapter, ConvertsVec3ToAndFromEigen) {
  const manim_cpp::math::Vec3 native = {2.5, -4.0, 9.0};

  const auto eigen = manim_cpp::math::to_eigen(native);
  const auto round_trip = manim_cpp::math::from_eigen(eigen);

  EXPECT_DOUBLE_EQ(round_trip[0], native[0]);
  EXPECT_DOUBLE_EQ(round_trip[1], native[1]);
  EXPECT_DOUBLE_EQ(round_trip[2], native[2]);
}

TEST(EigenAdapter, RotationMatrixMatchesRotateZ) {
  const manim_cpp::math::Vec3 point = {1.0, 0.0, 0.0};
  const auto expected = manim_cpp::math::rotate_z(point, manim_cpp::math::kPi / 2.0);

  const auto matrix = manim_cpp::math::rotation_z_matrix(manim_cpp::math::kPi / 2.0);
  const auto rotated = matrix * manim_cpp::math::to_eigen(point);

  EXPECT_NEAR(rotated(0), expected[0], 1e-9);
  EXPECT_NEAR(rotated(1), expected[1], 1e-9);
  EXPECT_NEAR(rotated(2), expected[2], 1e-9);
}
