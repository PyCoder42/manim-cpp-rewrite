#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "manim_cpp/math/core.hpp"
#include "manim_cpp/mobject/mobject.hpp"

namespace manim_cpp::mobject {

using GraphEdge = std::pair<std::string, std::string>;

class Graph : public Mobject {
 public:
  Graph(std::vector<std::string> vertices, std::vector<GraphEdge> edges);

  [[nodiscard]] std::string debug_name() const override { return "Graph"; }
  [[nodiscard]] const std::vector<std::string>& vertices() const;
  [[nodiscard]] const std::vector<GraphEdge>& edges() const;

  void set_layout_circular(double radius, double phase = 0.0);
  [[nodiscard]] std::optional<math::Vec3> vertex_position(
      const std::string& vertex_name) const;
  [[nodiscard]] const std::unordered_map<std::string, math::Vec3>&
  vertex_positions() const;

 private:
  std::vector<std::string> vertices_;
  std::vector<GraphEdge> edges_;
  std::unordered_map<std::string, math::Vec3> vertex_positions_;
};

}  // namespace manim_cpp::mobject
