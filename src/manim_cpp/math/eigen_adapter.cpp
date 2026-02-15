#include "manim_cpp/math/eigen_adapter.hpp"

#include <cmath>

namespace manim_cpp::math {

Eigen::Vector2d to_eigen(const Vec2& point) {
  return Eigen::Vector2d(point[0], point[1]);
}

Eigen::Vector3d to_eigen(const Vec3& point) {
  return Eigen::Vector3d(point[0], point[1], point[2]);
}

Vec2 from_eigen(const Eigen::Vector2d& point) {
  return Vec2{point.x(), point.y()};
}

Vec3 from_eigen(const Eigen::Vector3d& point) {
  return Vec3{point.x(), point.y(), point.z()};
}

Eigen::Matrix3d rotation_z_matrix(double radians) {
  const double c = std::cos(radians);
  const double s = std::sin(radians);

  Eigen::Matrix3d matrix;
  matrix << c, -s, 0.0, s, c, 0.0, 0.0, 0.0, 1.0;
  return matrix;
}

}  // namespace manim_cpp::math
