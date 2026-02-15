#include <memory>

#include <gtest/gtest.h>

#include "manim_cpp/animation/basic_animations.hpp"
#include "manim_cpp/math/core.hpp"
#include "manim_cpp/mobject/mobject.hpp"
#include "manim_cpp/scene/scene.hpp"

namespace {

using manim_cpp::animation::FadeToOpacityAnimation;
using manim_cpp::animation::MoveToAnimation;
using manim_cpp::animation::ShiftAnimation;
using manim_cpp::math::Vec3;
using manim_cpp::mobject::Mobject;

class EmptyScene final : public manim_cpp::scene::Scene {
 public:
  void construct() override {}
};

TEST(BasicAnimations, MoveToInterpolatesCenter) {
  auto mobject = std::make_shared<Mobject>();
  mobject->move_to(Vec3{-1.0, 2.0, 0.5});

  MoveToAnimation animation(mobject, Vec3{3.0, 6.0, 4.5});
  animation.begin();
  animation.interpolate(0.5);
  EXPECT_EQ(mobject->center(), (Vec3{1.0, 4.0, 2.5}));

  animation.interpolate(1.0);
  EXPECT_EQ(mobject->center(), (Vec3{3.0, 6.0, 4.5}));
}

TEST(BasicAnimations, ShiftInterpolatesByDelta) {
  auto mobject = std::make_shared<Mobject>();
  mobject->move_to(Vec3{1.0, -2.0, 0.0});

  ShiftAnimation animation(mobject, Vec3{2.0, 6.0, -1.0});
  animation.begin();
  animation.interpolate(0.25);
  EXPECT_EQ(mobject->center(), (Vec3{1.5, -0.5, -0.25}));

  animation.interpolate(1.0);
  EXPECT_EQ(mobject->center(), (Vec3{3.0, 4.0, -1.0}));
}

TEST(BasicAnimations, FadeToOpacityInterpolatesOpacity) {
  auto mobject = std::make_shared<Mobject>();
  mobject->set_opacity(1.0);

  FadeToOpacityAnimation animation(mobject, 0.2);
  animation.begin();
  animation.interpolate(0.5);
  EXPECT_NEAR(mobject->opacity(), 0.6, 1e-12);

  animation.interpolate(1.0);
  EXPECT_NEAR(mobject->opacity(), 0.2, 1e-12);
}

TEST(BasicAnimations, ScenePlayAppliesRunTimeAndRateFunction) {
  EmptyScene scene;
  auto mobject = std::make_shared<Mobject>();
  mobject->move_to(Vec3{0.0, 0.0, 0.0});
  scene.add(mobject);

  MoveToAnimation animation(mobject, Vec3{8.0, 0.0, 0.0});
  animation.set_run_time_seconds(2.0);
  animation.set_rate_function([](const double alpha) { return alpha * alpha; });

  scene.play(animation, 4);
  EXPECT_EQ(mobject->center(), (Vec3{8.0, 0.0, 0.0}));
  EXPECT_NEAR(scene.time_seconds(), 2.5, 1e-12);
}

}  // namespace
