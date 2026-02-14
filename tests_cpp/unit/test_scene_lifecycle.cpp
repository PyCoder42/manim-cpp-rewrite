#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/animation/animation.hpp"
#include "manim_cpp/scene/scene.hpp"

namespace {

class LifecycleProbeScene : public manim_cpp::scene::Scene {
 public:
  explicit LifecycleProbeScene(std::vector<std::string>* events) : events_(events) {}

  void setup() override { events_->push_back("setup"); }
  void construct() override { events_->push_back("construct"); }
  void tear_down() override { events_->push_back("tear_down"); }

 private:
  std::vector<std::string>* events_;
};

class AlphaRecordingAnimation : public manim_cpp::animation::Animation {
 public:
  void begin() override { began = true; }

  void interpolate(double alpha) override { alphas.push_back(alpha); }

  void finish() override { finished = true; }

  bool began = false;
  bool finished = false;
  std::vector<double> alphas;
};

class EmptyScene : public manim_cpp::scene::Scene {
 public:
  void construct() override {}
};

}  // namespace

TEST(SceneLifecycle, RunCallsLifecycleHooksInOrder) {
  std::vector<std::string> events;
  LifecycleProbeScene scene(&events);
  scene.run();

  const std::vector<std::string> expected = {"setup", "construct", "tear_down"};
  EXPECT_EQ(events, expected);
}

TEST(SceneLifecycle, PlayDrivesAnimationAndUpdaters) {
  EmptyScene scene;
  AlphaRecordingAnimation animation;

  double total_delta = 0.0;
  std::size_t updater_calls = 0;
  scene.add_updater([&](const double delta_seconds) {
    total_delta += delta_seconds;
    ++updater_calls;
  });

  scene.play(animation, 4);

  EXPECT_TRUE(animation.began);
  EXPECT_TRUE(animation.finished);
  ASSERT_EQ(animation.alphas.size(), static_cast<size_t>(5));
  EXPECT_DOUBLE_EQ(animation.alphas.front(), 0.0);
  EXPECT_DOUBLE_EQ(animation.alphas.back(), 1.0);
  EXPECT_EQ(updater_calls, static_cast<size_t>(5));
  EXPECT_NEAR(total_delta, 1.25, 1e-9);
  EXPECT_NEAR(scene.time_seconds(), 1.25, 1e-9);
}
