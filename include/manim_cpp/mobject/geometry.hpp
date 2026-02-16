#pragma once

#include <vector>

#include "manim_cpp/math/core.hpp"
#include "manim_cpp/mobject/mobject.hpp"

namespace manim_cpp::mobject {

class Dot : public Mobject {
 public:
  explicit Dot(double radius = 0.08);

  [[nodiscard]] std::string debug_name() const override { return "Dot"; }
  [[nodiscard]] double radius() const;
  void set_radius(double radius);

 private:
  double radius_ = 0.08;
};

class Circle : public Mobject {
 public:
  explicit Circle(double radius = 1.0);

  [[nodiscard]] std::string debug_name() const override { return "Circle"; }
  [[nodiscard]] double radius() const;
  void set_radius(double radius);
  [[nodiscard]] math::Vec3 point_at_angle(double angle_radians) const;

 private:
  double radius_ = 1.0;
};

class Square : public Mobject {
 public:
  explicit Square(double side_length = 2.0);

  [[nodiscard]] std::string debug_name() const override { return "Square"; }
  [[nodiscard]] double side_length() const;
  void set_side_length(double side_length);
  [[nodiscard]] std::vector<math::Vec3> vertices() const;

 private:
  double side_length_ = 2.0;
};

class Rectangle : public Mobject {
 public:
  Rectangle(double width = 4.0, double height = 2.0);

  [[nodiscard]] std::string debug_name() const override { return "Rectangle"; }
  [[nodiscard]] double width() const;
  [[nodiscard]] double height() const;
  void set_width(double width);
  void set_height(double height);
  [[nodiscard]] std::vector<math::Vec3> vertices() const;

 private:
  double width_ = 4.0;
  double height_ = 2.0;
};

class Line : public Mobject {
 public:
  Line(math::Vec3 start = math::Vec3{0.0, 0.0, 0.0},
       math::Vec3 end = math::Vec3{1.0, 0.0, 0.0});

  [[nodiscard]] std::string debug_name() const override { return "Line"; }
  [[nodiscard]] math::Vec3 start() const;
  [[nodiscard]] math::Vec3 end() const;
  [[nodiscard]] math::Vec3 unit_vector() const;
  [[nodiscard]] double length() const;
  void set_points(const math::Vec3& start, const math::Vec3& end);

 private:
  math::Vec3 unit_vector_{1.0, 0.0, 0.0};
  double length_ = 1.0;
};

}  // namespace manim_cpp::mobject
