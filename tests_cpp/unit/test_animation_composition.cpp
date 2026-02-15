#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/animation/composition.hpp"
#include "manim_cpp/scene/scene.hpp"

namespace {

class AlphaCaptureAnimation : public manim_cpp::animation::Animation {
 public:
  void begin() override { ++begin_calls; }
  void interpolate(double alpha) override { alphas.push_back(alpha); }
  void finish() override { ++finish_calls; }

  int begin_calls = 0;
  int finish_calls = 0;
  std::vector<double> alphas;
};

class EmptyScene final : public manim_cpp::scene::Scene {
 public:
  void construct() override {}
};

TEST(AnimationComposition, ParallelAnimationBroadcastsAlphaToAllAnimations) {
  AlphaCaptureAnimation first;
  AlphaCaptureAnimation second;
  first.set_rate_function([](double alpha) { return alpha * alpha; });

  manim_cpp::animation::ParallelAnimation parallel({&first, &second});
  parallel.begin();
  parallel.interpolate(0.5);
  parallel.finish();

  EXPECT_EQ(first.begin_calls, 1);
  EXPECT_EQ(second.begin_calls, 1);
  ASSERT_EQ(first.alphas.size(), static_cast<size_t>(1));
  ASSERT_EQ(second.alphas.size(), static_cast<size_t>(1));
  EXPECT_DOUBLE_EQ(first.alphas[0], 0.25);
  EXPECT_DOUBLE_EQ(second.alphas[0], 0.5);
  EXPECT_EQ(first.finish_calls, 1);
  EXPECT_EQ(second.finish_calls, 1);
}

TEST(AnimationComposition, SuccessionAnimationMapsGlobalTimeToLocalAlphas) {
  AlphaCaptureAnimation first;
  AlphaCaptureAnimation second;
  first.set_run_time_seconds(1.0);
  second.set_run_time_seconds(3.0);

  manim_cpp::animation::SuccessionAnimation succession({&first, &second});
  succession.begin();
  succession.interpolate(0.25);  // t=1.0
  succession.interpolate(0.50);  // t=2.0
  succession.interpolate(1.00);  // t=4.0
  succession.finish();

  ASSERT_EQ(first.alphas.size(), static_cast<size_t>(3));
  ASSERT_EQ(second.alphas.size(), static_cast<size_t>(3));
  EXPECT_DOUBLE_EQ(first.alphas[0], 1.0);
  EXPECT_DOUBLE_EQ(first.alphas[1], 1.0);
  EXPECT_DOUBLE_EQ(first.alphas[2], 1.0);
  EXPECT_DOUBLE_EQ(second.alphas[0], 0.0);
  EXPECT_NEAR(second.alphas[1], 1.0 / 3.0, 1e-12);
  EXPECT_DOUBLE_EQ(second.alphas[2], 1.0);
}

TEST(AnimationComposition, ScenePlayUsesComposedAnimationDuration) {
  AlphaCaptureAnimation first;
  AlphaCaptureAnimation second;
  first.set_run_time_seconds(1.5);
  second.set_run_time_seconds(0.5);

  manim_cpp::animation::ParallelAnimation parallel({&first, &second});
  EmptyScene scene;
  scene.play(parallel, 2);

  EXPECT_NEAR(scene.time_seconds(), 2.25, 1e-12);
}

}  // namespace
