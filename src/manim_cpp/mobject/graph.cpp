#include "manim_cpp/mobject/graph.hpp"

#include <unordered_set>

#include "manim_cpp/math/graph_layout.hpp"

namespace manim_cpp::mobject {

Graph::Graph(std::vector<std::string> vertices, std::vector<GraphEdge> edges) {
  std::unordered_set<std::string> seen_vertices;
  seen_vertices.reserve(vertices.size());
  for (const auto& vertex : vertices) {
    if (vertex.empty()) {
      continue;
    }
    if (seen_vertices.insert(vertex).second) {
      vertices_.push_back(vertex);
      vertex_positions_.insert({vertex, center()});
    }
  }

  for (const auto& edge : edges) {
    if (edge.first.empty() || edge.second.empty()) {
      continue;
    }
    if (!seen_vertices.contains(edge.first) || !seen_vertices.contains(edge.second)) {
      continue;
    }
    edges_.push_back(edge);
  }
}

const std::vector<std::string>& Graph::vertices() const {
  return vertices_;
}

const std::vector<GraphEdge>& Graph::edges() const {
  return edges_;
}

void Graph::set_layout_circular(const double radius, const double phase) {
  const auto layout = math::circular_layout(vertices_.size(), radius, phase);
  const auto& graph_center = center();
  for (std::size_t i = 0; i < vertices_.size(); ++i) {
    const auto& xy = layout[i];
    vertex_positions_[vertices_[i]] = math::Vec3{
        graph_center[0] + xy[0],
        graph_center[1] + xy[1],
        graph_center[2],
    };
  }
}

std::optional<math::Vec3> Graph::vertex_position(const std::string& vertex_name) const {
  const auto it = vertex_positions_.find(vertex_name);
  if (it == vertex_positions_.end()) {
    return std::nullopt;
  }
  return it->second;
}

const std::unordered_map<std::string, math::Vec3>& Graph::vertex_positions() const {
  return vertex_positions_;
}

}  // namespace manim_cpp::mobject
