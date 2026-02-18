#include <cmath>
#include <numbers>
#include <stdexcept>

#include <gtest/gtest.h>

#include "manim_cpp/mobject/geometry.hpp"

namespace {

using manim_cpp::math::Vec3;
using manim_cpp::mobject::Circle;
using manim_cpp::mobject::Dot;
using manim_cpp::mobject::Ellipse;
using manim_cpp::mobject::Annulus;
using manim_cpp::mobject::Line;
using manim_cpp::mobject::Arc;
using manim_cpp::mobject::Sector;
using manim_cpp::mobject::Rectangle;
using manim_cpp::mobject::RegularPolygon;
using manim_cpp::mobject::Square;
using manim_cpp::mobject::Triangle;

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

TEST(GeometryMobjects, EllipseComputesPointAtAngleFromCenter) {
  Ellipse ellipse(6.0, 2.0);
  ellipse.move_to(Vec3{1.0, 2.0, -3.0});

  EXPECT_DOUBLE_EQ(ellipse.width(), 6.0);
  EXPECT_DOUBLE_EQ(ellipse.height(), 2.0);

  const Vec3 right = ellipse.point_at_angle(0.0);
  expect_vec3_near(right, Vec3{4.0, 2.0, -3.0});

  const Vec3 top = ellipse.point_at_angle(std::numbers::pi / 2.0);
  expect_vec3_near(top, Vec3{1.0, 3.0, -3.0});

  EXPECT_THROW(ellipse.set_width(0.0), std::invalid_argument);
  EXPECT_THROW(ellipse.set_height(0.0), std::invalid_argument);
}

TEST(GeometryMobjects, ArcTracksRadiusAndAngles) {
  Arc arc(2.0, 0.0, std::numbers::pi / 2.0);
  arc.move_to(Vec3{1.0, -1.0, 0.5});

  EXPECT_DOUBLE_EQ(arc.radius(), 2.0);
  EXPECT_DOUBLE_EQ(arc.start_angle(), 0.0);
  EXPECT_DOUBLE_EQ(arc.angle(), std::numbers::pi / 2.0);

  expect_vec3_near(arc.start_point(), Vec3{3.0, -1.0, 0.5});
  expect_vec3_near(arc.end_point(), Vec3{1.0, 1.0, 0.5});
  expect_vec3_near(arc.point_at_proportion(0.5),
                   Vec3{1.0 + std::sqrt(2.0), -1.0 + std::sqrt(2.0), 0.5});
}

TEST(GeometryMobjects, ArcRejectsNonPositiveRadius) {
  EXPECT_THROW(Arc(0.0, 0.0, std::numbers::pi / 2.0), std::invalid_argument);

  Arc arc;
  EXPECT_THROW(arc.set_radius(0.0), std::invalid_argument);
}

TEST(GeometryMobjects, AnnulusTracksInnerAndOuterRadii) {
  Annulus annulus(0.75, 2.0);
  annulus.move_to(Vec3{-1.0, 2.0, 0.25});

  EXPECT_DOUBLE_EQ(annulus.inner_radius(), 0.75);
  EXPECT_DOUBLE_EQ(annulus.outer_radius(), 2.0);
  expect_vec3_near(annulus.outer_point_at_angle(0.0), Vec3{1.0, 2.0, 0.25});
  expect_vec3_near(annulus.inner_point_at_angle(std::numbers::pi / 2.0),
                   Vec3{-1.0, 2.75, 0.25});

  annulus.set_radii(0.5, 1.5);
  EXPECT_DOUBLE_EQ(annulus.inner_radius(), 0.5);
  EXPECT_DOUBLE_EQ(annulus.outer_radius(), 1.5);
}

TEST(GeometryMobjects, AnnulusRejectsInvalidRadii) {
  EXPECT_THROW(Annulus(0.0, 1.0), std::invalid_argument);
  EXPECT_THROW(Annulus(1.0, 1.0), std::invalid_argument);
  EXPECT_THROW(Annulus(1.2, 1.0), std::invalid_argument);

  Annulus annulus;
  EXPECT_THROW(annulus.set_radii(0.0, 2.0), std::invalid_argument);
  EXPECT_THROW(annulus.set_radii(1.0, 1.0), std::invalid_argument);
}

TEST(GeometryMobjects, SectorTracksRadiiAnglesAndBoundaryPoints) {
  Sector sector(0.5, 2.0, std::numbers::pi / 4.0, std::numbers::pi / 2.0);
  sector.move_to(Vec3{1.0, -1.0, 0.0});

  EXPECT_DOUBLE_EQ(sector.inner_radius(), 0.5);
  EXPECT_DOUBLE_EQ(sector.outer_radius(), 2.0);
  EXPECT_DOUBLE_EQ(sector.start_angle(), std::numbers::pi / 4.0);
  EXPECT_DOUBLE_EQ(sector.angle(), std::numbers::pi / 2.0);
  expect_vec3_near(
      sector.outer_start_point(),
      Vec3{1.0 + std::sqrt(2.0), -1.0 + std::sqrt(2.0), 0.0});
  expect_vec3_near(
      sector.outer_end_point(),
      Vec3{1.0 - std::sqrt(2.0), -1.0 + std::sqrt(2.0), 0.0});
  expect_vec3_near(
      sector.inner_start_point(),
      Vec3{1.0 + (std::sqrt(2.0) / 4.0), -1.0 + (std::sqrt(2.0) / 4.0), 0.0});
}

TEST(GeometryMobjects, SectorRejectsInvalidRadiusConfiguration) {
  EXPECT_THROW(Sector(-0.1, 2.0, 0.0, std::numbers::pi / 2.0), std::invalid_argument);
  EXPECT_THROW(Sector(0.5, 0.0, 0.0, std::numbers::pi / 2.0), std::invalid_argument);
  EXPECT_THROW(Sector(1.0, 1.0, 0.0, std::numbers::pi / 2.0), std::invalid_argument);

  Sector sector;
  EXPECT_THROW(sector.set_radii(-1.0, 1.0), std::invalid_argument);
  EXPECT_THROW(sector.set_radii(1.0, 1.0), std::invalid_argument);
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

TEST(GeometryMobjects, RectangleVerticesTrackCenterWidthAndHeight) {
  Rectangle rectangle(6.0, 2.0);
  rectangle.move_to(Vec3{-2.0, 3.0, 1.0});

  EXPECT_DOUBLE_EQ(rectangle.width(), 6.0);
  EXPECT_DOUBLE_EQ(rectangle.height(), 2.0);

  const auto vertices = rectangle.vertices();
  ASSERT_EQ(vertices.size(), static_cast<size_t>(4));
  expect_vec3_near(vertices[0], Vec3{-5.0, 2.0, 1.0});
  expect_vec3_near(vertices[1], Vec3{1.0, 2.0, 1.0});
  expect_vec3_near(vertices[2], Vec3{1.0, 4.0, 1.0});
  expect_vec3_near(vertices[3], Vec3{-5.0, 4.0, 1.0});
}

TEST(GeometryMobjects, RectangleRejectsNonPositiveDimensions) {
  EXPECT_THROW(Rectangle(0.0, 2.0), std::invalid_argument);
  EXPECT_THROW(Rectangle(2.0, -1.0), std::invalid_argument);

  Rectangle rectangle;
  EXPECT_THROW(rectangle.set_width(0.0), std::invalid_argument);
  EXPECT_THROW(rectangle.set_height(0.0), std::invalid_argument);
}

TEST(GeometryMobjects, TriangleVerticesTrackCenterAndSideLength) {
  Triangle triangle(2.0);
  triangle.move_to(Vec3{1.0, -2.0, 0.5});

  EXPECT_DOUBLE_EQ(triangle.side_length(), 2.0);

  const auto vertices = triangle.vertices();
  ASSERT_EQ(vertices.size(), static_cast<size_t>(3));
  expect_vec3_near(vertices[0], Vec3{1.0, -0.8452994616207485, 0.5});
  expect_vec3_near(vertices[1], Vec3{0.0, -2.5773502691896257, 0.5});
  expect_vec3_near(vertices[2], Vec3{2.0, -2.5773502691896257, 0.5});
}

TEST(GeometryMobjects, TriangleRejectsNonPositiveSideLength) {
  EXPECT_THROW(Triangle(0.0), std::invalid_argument);
  EXPECT_THROW(Triangle(-2.0), std::invalid_argument);

  Triangle triangle;
  EXPECT_THROW(triangle.set_side_length(0.0), std::invalid_argument);
}

TEST(GeometryMobjects, RegularPolygonVerticesTrackCenterSidesAndRadius) {
  RegularPolygon polygon(5, 2.0);
  polygon.move_to(Vec3{1.0, -1.0, 0.25});

  EXPECT_EQ(polygon.n_sides(), static_cast<size_t>(5));
  EXPECT_DOUBLE_EQ(polygon.radius(), 2.0);

  const auto vertices = polygon.vertices();
  ASSERT_EQ(vertices.size(), static_cast<size_t>(5));
  expect_vec3_near(vertices[0], Vec3{1.0, 1.0, 0.25});
  expect_vec3_near(vertices[1], Vec3{-0.9021130325903071, -0.3819660112501051, 0.25});
  expect_vec3_near(vertices[2], Vec3{-0.17557050458494672, -2.618033988749895, 0.25});
  expect_vec3_near(vertices[3], Vec3{2.175570504584946, -2.618033988749895, 0.25});
  expect_vec3_near(vertices[4], Vec3{2.9021130325903073, -0.38196601125010554, 0.25});
}

TEST(GeometryMobjects, RegularPolygonRejectsInvalidSidesAndRadius) {
  EXPECT_THROW(RegularPolygon(2, 1.0), std::invalid_argument);
  EXPECT_THROW(RegularPolygon(5, 0.0), std::invalid_argument);

  RegularPolygon polygon;
  EXPECT_THROW(polygon.set_n_sides(2), std::invalid_argument);
  EXPECT_THROW(polygon.set_radius(0.0), std::invalid_argument);
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
