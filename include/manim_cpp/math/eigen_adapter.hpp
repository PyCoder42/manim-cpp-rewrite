#pragma once

#include <Eigen/Core>

#include "manim_cpp/math/core.hpp"

namespace manim_cpp::math {

Eigen::Vector2d to_eigen(const Vec2& point);
Eigen::Vector3d to_eigen(const Vec3& point);

Vec2 from_eigen(const Eigen::Vector2d& point);
Vec3 from_eigen(const Eigen::Vector3d& point);

Eigen::Matrix3d rotation_z_matrix(double radians);

}  // namespace manim_cpp::math
