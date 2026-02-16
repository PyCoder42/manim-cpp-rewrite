#include "manim_cpp/mobject/geometry.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

namespace manim_cpp::mobject {

namespace {

double require_positive(const double value, const char* field_name) {
  if (value <= 0.0) {
    throw std::invalid_argument(std::string(field_name) + " must be positive");
  }
  return value;
}

math::Vec3 subtract(const math::Vec3& a, const math::Vec3& b) {
  return math::Vec3{a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}

math::Vec3 scale(const math::Vec3& a, const double factor) {
  return math::Vec3{a[0] * factor, a[1] * factor, a[2] * factor};
}

math::Vec3 midpoint(const math::Vec3& a, const math::Vec3& b) {
  return math::Vec3{(a[0] + b[0]) / 2.0, (a[1] + b[1]) / 2.0, (a[2] + b[2]) / 2.0};
}

double norm(const math::Vec3& a) {
  return std::sqrt((a[0] * a[0]) + (a[1] * a[1]) + (a[2] * a[2]));
}

}  // namespace

Dot::Dot(const double radius) : radius_(require_positive(radius, "radius")) {}

double Dot::radius() const { return radius_; }

void Dot::set_radius(const double radius) {
  radius_ = require_positive(radius, "radius");
}

Circle::Circle(const double radius) : radius_(require_positive(radius, "radius")) {}

double Circle::radius() const { return radius_; }

void Circle::set_radius(const double radius) {
  radius_ = require_positive(radius, "radius");
}

math::Vec3 Circle::point_at_angle(const double angle_radians) const {
  const auto& c = center();
  return math::Vec3{
      c[0] + (radius_ * std::cos(angle_radians)),
      c[1] + (radius_ * std::sin(angle_radians)),
      c[2],
  };
}

Square::Square(const double side_length)
    : side_length_(require_positive(side_length, "side_length")) {}

double Square::side_length() const { return side_length_; }

void Square::set_side_length(const double side_length) {
  side_length_ = require_positive(side_length, "side_length");
}

std::vector<math::Vec3> Square::vertices() const {
  const auto& c = center();
  const double half = side_length_ / 2.0;
  return {
      math::Vec3{c[0] - half, c[1] - half, c[2]},
      math::Vec3{c[0] + half, c[1] - half, c[2]},
      math::Vec3{c[0] + half, c[1] + half, c[2]},
      math::Vec3{c[0] - half, c[1] + half, c[2]},
  };
}

Rectangle::Rectangle(const double width, const double height)
    : width_(require_positive(width, "width")),
      height_(require_positive(height, "height")) {}

double Rectangle::width() const { return width_; }

double Rectangle::height() const { return height_; }

void Rectangle::set_width(const double width) {
  width_ = require_positive(width, "width");
}

void Rectangle::set_height(const double height) {
  height_ = require_positive(height, "height");
}

std::vector<math::Vec3> Rectangle::vertices() const {
  const auto& c = center();
  const double half_width = width_ / 2.0;
  const double half_height = height_ / 2.0;
  return {
      math::Vec3{c[0] - half_width, c[1] - half_height, c[2]},
      math::Vec3{c[0] + half_width, c[1] - half_height, c[2]},
      math::Vec3{c[0] + half_width, c[1] + half_height, c[2]},
      math::Vec3{c[0] - half_width, c[1] + half_height, c[2]},
  };
}

Line::Line(const math::Vec3 start, const math::Vec3 end) {
  set_points(start, end);
}

math::Vec3 Line::start() const {
  const auto half = scale(unit_vector_, length_ / 2.0);
  return subtract(center(), half);
}

math::Vec3 Line::end() const {
  const auto half = scale(unit_vector_, length_ / 2.0);
  const auto& c = center();
  return math::Vec3{c[0] + half[0], c[1] + half[1], c[2] + half[2]};
}

math::Vec3 Line::unit_vector() const { return unit_vector_; }

double Line::length() const { return length_; }

void Line::set_points(const math::Vec3& start, const math::Vec3& end) {
  const auto delta = subtract(end, start);
  const auto length = norm(delta);
  if (length <= 0.0) {
    throw std::invalid_argument("line endpoints must be distinct");
  }

  length_ = length;
  unit_vector_ = scale(delta, 1.0 / length_);
  move_to(midpoint(start, end));
}

}  // namespace manim_cpp::mobject
