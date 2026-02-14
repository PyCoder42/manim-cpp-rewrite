#include "manim_cpp/math/path_ops.hpp"

#include <algorithm>
#include <cmath>

namespace manim_cpp::math {
namespace {

constexpr double kEpsilon = 1e-12;

double orientation(const Vec2& a, const Vec2& b, const Vec2& c) {
  return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
}

bool on_segment(const Vec2& a, const Vec2& b, const Vec2& point) {
  if (std::abs(orientation(a, b, point)) > kEpsilon) {
    return false;
  }

  return point[0] >= std::min(a[0], b[0]) - kEpsilon &&
         point[0] <= std::max(a[0], b[0]) + kEpsilon &&
         point[1] >= std::min(a[1], b[1]) - kEpsilon &&
         point[1] <= std::max(a[1], b[1]) + kEpsilon;
}

bool is_inside_clip_edge(const Vec2& point,
                         const Vec2& edge_start,
                         const Vec2& edge_end,
                         const double orientation_sign) {
  return orientation_sign * orientation(edge_start, edge_end, point) >= -kEpsilon;
}

Vec2 intersect_lines(const Vec2& p1, const Vec2& p2, const Vec2& q1, const Vec2& q2) {
  const double a1 = p2[1] - p1[1];
  const double b1 = p1[0] - p2[0];
  const double c1 = (a1 * p1[0]) + (b1 * p1[1]);

  const double a2 = q2[1] - q1[1];
  const double b2 = q1[0] - q2[0];
  const double c2 = (a2 * q1[0]) + (b2 * q1[1]);

  const double det = (a1 * b2) - (a2 * b1);
  if (std::abs(det) <= kEpsilon) {
    return p2;
  }

  return Vec2{
      ((b2 * c1) - (b1 * c2)) / det,
      ((a1 * c2) - (a2 * c1)) / det,
  };
}

}  // namespace

bool segments_intersect(const Vec2& a0, const Vec2& a1, const Vec2& b0, const Vec2& b1) {
  const double o1 = orientation(a0, a1, b0);
  const double o2 = orientation(a0, a1, b1);
  const double o3 = orientation(b0, b1, a0);
  const double o4 = orientation(b0, b1, a1);

  const bool proper_intersection =
      ((o1 > kEpsilon && o2 < -kEpsilon) || (o1 < -kEpsilon && o2 > kEpsilon)) &&
      ((o3 > kEpsilon && o4 < -kEpsilon) || (o3 < -kEpsilon && o4 > kEpsilon));

  if (proper_intersection) {
    return true;
  }

  if (on_segment(a0, a1, b0) || on_segment(a0, a1, b1) || on_segment(b0, b1, a0) ||
      on_segment(b0, b1, a1)) {
    return true;
  }

  return false;
}

bool point_in_polygon(const std::vector<Vec2>& polygon, const Vec2& point) {
  if (polygon.size() < 3) {
    return false;
  }

  bool inside = false;
  for (size_t i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
    const auto& pi = polygon[i];
    const auto& pj = polygon[j];

    if (on_segment(pj, pi, point)) {
      return true;
    }

    const bool crosses_vertical_span = ((pi[1] > point[1]) != (pj[1] > point[1]));
    if (!crosses_vertical_span) {
      continue;
    }

    const double x_on_edge = ((pj[0] - pi[0]) * (point[1] - pi[1])) / (pj[1] - pi[1]) + pi[0];
    if (point[0] < x_on_edge) {
      inside = !inside;
    }
  }

  return inside;
}

bool has_self_intersections(const std::vector<Vec2>& polygon) {
  if (polygon.size() < 4) {
    return false;
  }

  const size_t edge_count = polygon.size();
  for (size_t i = 0; i < edge_count; ++i) {
    const auto& a0 = polygon[i];
    const auto& a1 = polygon[(i + 1) % edge_count];

    for (size_t j = i + 1; j < edge_count; ++j) {
      if (i == j || (i + 1) % edge_count == j || i == (j + 1) % edge_count) {
        continue;
      }

      const auto& b0 = polygon[j];
      const auto& b1 = polygon[(j + 1) % edge_count];
      if (segments_intersect(a0, a1, b0, b1)) {
        return true;
      }
    }
  }

  return false;
}

double polygon_signed_area(const std::vector<Vec2>& polygon) {
  if (polygon.size() < 3) {
    return 0.0;
  }

  double sum = 0.0;
  for (size_t i = 0; i < polygon.size(); ++i) {
    const auto& a = polygon[i];
    const auto& b = polygon[(i + 1) % polygon.size()];
    sum += (a[0] * b[1]) - (b[0] * a[1]);
  }
  return 0.5 * sum;
}

double polygon_area(const std::vector<Vec2>& polygon) {
  return std::abs(polygon_signed_area(polygon));
}

std::vector<Vec2> intersect_convex_polygons(const std::vector<Vec2>& subject,
                                            const std::vector<Vec2>& clip) {
  if (subject.size() < 3 || clip.size() < 3) {
    return {};
  }

  std::vector<Vec2> output = subject;
  const double orientation_sign = polygon_signed_area(clip) >= 0.0 ? 1.0 : -1.0;

  for (size_t i = 0; i < clip.size(); ++i) {
    const auto& edge_start = clip[i];
    const auto& edge_end = clip[(i + 1) % clip.size()];
    const auto input = output;
    output.clear();
    if (input.empty()) {
      break;
    }

    Vec2 previous = input.back();
    bool previous_inside =
        is_inside_clip_edge(previous, edge_start, edge_end, orientation_sign);
    for (const auto& current : input) {
      const bool current_inside =
          is_inside_clip_edge(current, edge_start, edge_end, orientation_sign);

      if (current_inside) {
        if (!previous_inside) {
          output.push_back(intersect_lines(previous, current, edge_start, edge_end));
        }
        output.push_back(current);
      } else if (previous_inside) {
        output.push_back(intersect_lines(previous, current, edge_start, edge_end));
      }

      previous = current;
      previous_inside = current_inside;
    }
  }

  return output;
}

double union_area_convex_polygons(const std::vector<Vec2>& subject,
                                  const std::vector<Vec2>& clip) {
  const double subject_area = polygon_area(subject);
  const double clip_area = polygon_area(clip);
  if (subject_area <= kEpsilon) {
    return clip_area;
  }
  if (clip_area <= kEpsilon) {
    return subject_area;
  }

  const auto intersection = intersect_convex_polygons(subject, clip);
  const double intersection_area = polygon_area(intersection);
  return subject_area + clip_area - intersection_area;
}

double difference_area_convex_polygons(const std::vector<Vec2>& subject,
                                       const std::vector<Vec2>& clip) {
  const double subject_area = polygon_area(subject);
  if (subject_area <= kEpsilon) {
    return 0.0;
  }

  const auto intersection = intersect_convex_polygons(subject, clip);
  const double remaining_area = subject_area - polygon_area(intersection);
  if (remaining_area <= kEpsilon) {
    return 0.0;
  }
  return remaining_area;
}

}  // namespace manim_cpp::math
