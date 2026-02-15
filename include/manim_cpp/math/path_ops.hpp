#pragma once

#include <vector>

#include "manim_cpp/math/core.hpp"

namespace manim_cpp::math {

bool segments_intersect(const Vec2& a0, const Vec2& a1, const Vec2& b0, const Vec2& b1);

bool point_in_polygon(const std::vector<Vec2>& polygon, const Vec2& point);

bool has_self_intersections(const std::vector<Vec2>& polygon);

double polygon_signed_area(const std::vector<Vec2>& polygon);
double polygon_area(const std::vector<Vec2>& polygon);

std::vector<Vec2> intersect_convex_polygons(const std::vector<Vec2>& subject,
                                            const std::vector<Vec2>& clip);

double union_area_convex_polygons(const std::vector<Vec2>& subject,
                                  const std::vector<Vec2>& clip);

double difference_area_convex_polygons(const std::vector<Vec2>& subject,
                                       const std::vector<Vec2>& clip);

}  // namespace manim_cpp::math
