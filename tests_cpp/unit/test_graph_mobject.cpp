#include <optional>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/mobject/graph.hpp"

namespace {

using manim_cpp::math::Vec3;
using manim_cpp::mobject::Graph;
using manim_cpp::mobject::GraphEdge;

TEST(GraphMobject, KeepsUniqueVerticesAndValidEdges) {
  Graph graph({"A", "B", "A", "C"},
              {
                  GraphEdge{"A", "B"},
                  GraphEdge{"B", "C"},
                  GraphEdge{"A", "D"},
                  GraphEdge{"", "C"},
              });

  ASSERT_EQ(graph.vertices().size(), static_cast<size_t>(3));
  EXPECT_EQ(graph.vertices()[0], std::string("A"));
  EXPECT_EQ(graph.vertices()[1], std::string("B"));
  EXPECT_EQ(graph.vertices()[2], std::string("C"));

  ASSERT_EQ(graph.edges().size(), static_cast<size_t>(2));
  EXPECT_EQ(graph.edges()[0], (GraphEdge{"A", "B"}));
  EXPECT_EQ(graph.edges()[1], (GraphEdge{"B", "C"}));
}

TEST(GraphMobject, CircularLayoutAssignsDeterministicVertexPositions) {
  Graph graph({"A", "B", "C", "D"},
              {GraphEdge{"A", "B"}, GraphEdge{"B", "C"}, GraphEdge{"C", "D"}});
  graph.set_layout_circular(2.0);

  const auto a = graph.vertex_position("A");
  const auto b = graph.vertex_position("B");
  const auto c = graph.vertex_position("C");
  const auto d = graph.vertex_position("D");
  ASSERT_TRUE(a.has_value());
  ASSERT_TRUE(b.has_value());
  ASSERT_TRUE(c.has_value());
  ASSERT_TRUE(d.has_value());

  EXPECT_NEAR((*a)[0], 2.0, 1e-9);
  EXPECT_NEAR((*a)[1], 0.0, 1e-9);
  EXPECT_NEAR((*b)[0], 0.0, 1e-9);
  EXPECT_NEAR((*b)[1], 2.0, 1e-9);
  EXPECT_NEAR((*c)[0], -2.0, 1e-9);
  EXPECT_NEAR((*c)[1], 0.0, 1e-9);
  EXPECT_NEAR((*d)[0], 0.0, 1e-9);
  EXPECT_NEAR((*d)[1], -2.0, 1e-9);
}

TEST(GraphMobject, LayoutRespectsGraphCenterOffset) {
  Graph graph({"A"}, {});
  graph.move_to(Vec3{5.0, -3.0, 2.0});
  graph.set_layout_circular(1.5, manim_cpp::math::kPi / 2.0);

  const auto position = graph.vertex_position("A");
  ASSERT_TRUE(position.has_value());
  EXPECT_NEAR((*position)[0], 5.0, 1e-9);
  EXPECT_NEAR((*position)[1], -1.5, 1e-9);
  EXPECT_NEAR((*position)[2], 2.0, 1e-9);
}

TEST(GraphMobject, ReturnsNulloptForUnknownVertexPosition) {
  Graph graph({"A", "B"}, {GraphEdge{"A", "B"}});
  EXPECT_EQ(graph.vertex_position("Missing"), std::nullopt);
}

}  // namespace
