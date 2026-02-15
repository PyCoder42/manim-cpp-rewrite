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

}  // namespace manim_cpp::mobject
