#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/scene/scene.hpp"

namespace {

class EmptyScene : public manim_cpp::scene::Scene {
 public:
  void construct() override {}
};

}  // namespace

TEST(SceneRandom, SeededSequencesAreStableAcrossInstances) {
  EmptyScene first;
  EmptyScene second;
  first.set_random_seed(1337);
  second.set_random_seed(1337);

  std::vector<double> seq_a;
  std::vector<double> seq_b;
  for (size_t index = 0; index < 5; ++index) {
    seq_a.push_back(first.random_unit());
    seq_b.push_back(second.random_unit());
  }

  EXPECT_EQ(seq_a, seq_b);
}

TEST(SceneRandom, ResettingSeedReplaysSequence) {
  EmptyScene scene;
  scene.set_random_seed(7);
  const double first = scene.random_unit();
  const double second = scene.random_unit();

  scene.set_random_seed(7);
  EXPECT_DOUBLE_EQ(scene.random_unit(), first);
  EXPECT_DOUBLE_EQ(scene.random_unit(), second);
}
