#include <cmath>
#include <numbers>
#include <stdexcept>

#include <gtest/gtest.h>

#include "manim_cpp/mobject/geometry.hpp"

namespace {

using manim_cpp::math::Vec3;
using manim_cpp::mobject::Circle;
using manim_cpp::mobject::Dot;
using manim_cpp::mobject::Line;
using manim_cpp::mobject::Square;

void expect_vec3_near(const Vec3& actual, const Vec3& expected, const double eps = 1e-12) {
  EXPECT_NEAR(actual[0], expected[0], eps);
  EXPECT_NEAR(actual[1], expected[1], eps);
  EXPECT_NEAR(actual[2], expected[2], eps);
}

TEST(GeometryMobjects, DotMaintainsPositiveRadius) {
  Dot dot;
  EXPECT_DOUBLE_EQ(dot.radius(), 0.08);
  dot.set_radius(0.25);
  EXPECT_DOUBLE_EQ(dot.radius(), 0.25);
  EXPECT_THROW(dot.set_radius(0.0), std::invalid_argument);
  EXPECT_THROW(Dot(-1.0), std::invalid_argument);
}

TEST(GeometryMobjects, CircleComputesPointAtAngleFromCenter) {
  Circle circle(2.0);
  circle.move_to(Vec3{1.0, 3.0, -1.0});

  const Vec3 right = circle.point_at_angle(0.0);
  EXPECT_NEAR(right[0], 3.0, 1e-12);
  EXPECT_NEAR(right[1], 3.0, 1e-12);
  EXPECT_NEAR(right[2], -1.0, 1e-12);

  const Vec3 top = circle.point_at_angle(std::numbers::pi / 2.0);
  EXPECT_NEAR(top[0], 1.0, 1e-12);
  EXPECT_NEAR(top[1], 5.0, 1e-12);
  EXPECT_NEAR(top[2], -1.0, 1e-12);

  EXPECT_THROW(circle.set_radius(-0.5), std::invalid_argument);
}

TEST(GeometryMobjects, SquareVerticesTrackCenterAndSideLength) {
  Square square(4.0);
  square.move_to(Vec3{2.0, -1.0, 0.5});

  const auto vertices = square.vertices();
  ASSERT_EQ(vertices.size(), static_cast<size_t>(4));
  EXPECT_EQ(vertices[0], (Vec3{0.0, -3.0, 0.5}));
  EXPECT_EQ(vertices[1], (Vec3{4.0, -3.0, 0.5}));
  EXPECT_EQ(vertices[2], (Vec3{4.0, 1.0, 0.5}));
  EXPECT_EQ(vertices[3], (Vec3{0.0, 1.0, 0.5}));

  square.set_side_length(1.0);
  const auto smaller_vertices = square.vertices();
  EXPECT_EQ(smaller_vertices[0], (Vec3{1.5, -1.5, 0.5}));
  EXPECT_THROW(square.set_side_length(0.0), std::invalid_argument);
}

TEST(GeometryMobjects, LineTracksEndpointsLengthAndDirection) {
  Line line(Vec3{-2.0, 1.0, 0.0}, Vec3{4.0, 1.0, 0.0});

  expect_vec3_near(line.start(), Vec3{-2.0, 1.0, 0.0});
  expect_vec3_near(line.end(), Vec3{4.0, 1.0, 0.0});
  expect_vec3_near(line.center(), Vec3{1.0, 1.0, 0.0});
  EXPECT_NEAR(line.length(), 6.0, 1e-12);
  expect_vec3_near(line.unit_vector(), Vec3{1.0, 0.0, 0.0});
}

TEST(GeometryMobjects, LineUpdatesEndpointsWhenMovedAndReparameterized) {
  Line line(Vec3{0.0, 0.0, 0.0}, Vec3{0.0, 2.0, 0.0});

  line.shift(Vec3{2.0, -1.0, 3.0});
  expect_vec3_near(line.start(), Vec3{2.0, -1.0, 3.0});
  expect_vec3_near(line.end(), Vec3{2.0, 1.0, 3.0});

  line.move_to(Vec3{-3.0, 0.0, 2.0});
  expect_vec3_near(line.start(), Vec3{-3.0, -1.0, 2.0});
  expect_vec3_near(line.end(), Vec3{-3.0, 1.0, 2.0});

  line.set_points(Vec3{1.0, 2.0, 3.0}, Vec3{4.0, 6.0, 3.0});
  EXPECT_NEAR(line.length(), 5.0, 1e-12);
  expect_vec3_near(line.start(), Vec3{1.0, 2.0, 3.0});
  expect_vec3_near(line.end(), Vec3{4.0, 6.0, 3.0});
  expect_vec3_near(line.unit_vector(), Vec3{0.6, 0.8, 0.0});
}

TEST(GeometryMobjects, LineRejectsDegenerateEndpoints) {
  EXPECT_THROW(Line(Vec3{1.0, 1.0, 1.0}, Vec3{1.0, 1.0, 1.0}), std::invalid_argument);

  Line line;
  EXPECT_THROW(line.set_points(Vec3{2.0, 2.0, 2.0}, Vec3{2.0, 2.0, 2.0}),
               std::invalid_argument);
}

}  // namespace
