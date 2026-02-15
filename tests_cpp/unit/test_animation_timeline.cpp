#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/animation/animation.hpp"
#include "manim_cpp/scene/scene.hpp"

namespace {

class AlphaCaptureAnimation : public manim_cpp::animation::Animation {
 public:
  void interpolate(double alpha) override { alphas.push_back(alpha); }
  std::vector<double> alphas;
};

class EmptyScene : public manim_cpp::scene::Scene {
 public:
  void construct() override {}
};

}  // namespace

TEST(AnimationTimeline, RateFunctionTransformsInterpolatedAlpha) {
  AlphaCaptureAnimation animation;
  animation.set_rate_function([](const double alpha) { return alpha * alpha; });

  animation.interpolate_with_rate(0.5);
  ASSERT_EQ(animation.alphas.size(), static_cast<size_t>(1));
  EXPECT_DOUBLE_EQ(animation.alphas[0], 0.25);
}

TEST(AnimationTimeline, ScenePlayUsesAnimationRunTime) {
  EmptyScene scene;
  AlphaCaptureAnimation animation;
  animation.set_run_time_seconds(2.0);

  scene.play(animation, 4);
  EXPECT_NEAR(scene.time_seconds(), 2.5, 1e-9);
}
