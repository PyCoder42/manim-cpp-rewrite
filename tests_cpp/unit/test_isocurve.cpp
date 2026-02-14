#include <cmath>
#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/math/isocurve.hpp"

namespace {

bool approx_equal(const manim_cpp::math::Vec2& a, const manim_cpp::math::Vec2& b) {
  return std::abs(a[0] - b[0]) < 1e-9 && std::abs(a[1] - b[1]) < 1e-9;
}

}  // namespace

TEST(Isocurve, ExtractsSingleVerticalSegmentFromOneCell) {
  const std::vector<std::vector<double>> field = {
      {0.0, 1.0},
      {0.0, 1.0},
  };

  const auto segments = manim_cpp::math::extract_isocurve_segments(field, 0.5);
  ASSERT_EQ(segments.size(), static_cast<size_t>(1));

  const manim_cpp::math::Vec2 expected_a = {0.5, 0.0};
  const manim_cpp::math::Vec2 expected_b = {0.5, 1.0};

  const auto& segment = segments[0];
  const bool matches_direct = approx_equal(segment.start, expected_a) &&
                              approx_equal(segment.end, expected_b);
  const bool matches_reversed = approx_equal(segment.start, expected_b) &&
                                approx_equal(segment.end, expected_a);
  EXPECT_TRUE(matches_direct || matches_reversed);
}

TEST(Isocurve, ReturnsNoSegmentsWhenNoCrossingsExist) {
  const std::vector<std::vector<double>> field = {
      {0.0, 0.0},
      {0.0, 0.0},
  };

  const auto segments = manim_cpp::math::extract_isocurve_segments(field, 0.5);
  EXPECT_TRUE(segments.empty());
}
