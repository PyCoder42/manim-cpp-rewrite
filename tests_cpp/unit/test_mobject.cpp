#include <memory>

#include <gtest/gtest.h>

#include "manim_cpp/mobject/mobject.hpp"

namespace {

using manim_cpp::math::Vec3;
using manim_cpp::mobject::Mobject;

TEST(Mobject, MaintainsUniqueChildOrderAndRemoval) {
  auto parent = std::make_shared<Mobject>();
  auto first = std::make_shared<Mobject>();
  auto second = std::make_shared<Mobject>();

  parent->add(first);
  parent->add(second);
  parent->add(first);

  ASSERT_EQ(parent->submobjects().size(), static_cast<size_t>(2));
  EXPECT_EQ(parent->submobjects()[0], first);
  EXPECT_EQ(parent->submobjects()[1], second);

  EXPECT_TRUE(parent->remove(first));
  EXPECT_FALSE(parent->remove(first));
  ASSERT_EQ(parent->submobjects().size(), static_cast<size_t>(1));
  EXPECT_EQ(parent->submobjects()[0], second);

  parent->clear_submobjects();
  EXPECT_TRUE(parent->submobjects().empty());
}

TEST(Mobject, TracksCenterUsingMoveToAndShift) {
  Mobject mobject;
  EXPECT_EQ(mobject.center(), (Vec3{0.0, 0.0, 0.0}));

  mobject.move_to(Vec3{1.0, 2.0, 3.0});
  EXPECT_EQ(mobject.center(), (Vec3{1.0, 2.0, 3.0}));

  mobject.shift(Vec3{-0.5, 0.5, 1.0});
  EXPECT_EQ(mobject.center(), (Vec3{0.5, 2.5, 4.0}));
}

TEST(Mobject, ClampsOpacityIntoZeroToOneRange) {
  Mobject mobject;
  EXPECT_DOUBLE_EQ(mobject.opacity(), 1.0);

  mobject.set_opacity(0.25);
  EXPECT_DOUBLE_EQ(mobject.opacity(), 0.25);

  mobject.set_opacity(-0.3);
  EXPECT_DOUBLE_EQ(mobject.opacity(), 0.0);

  mobject.set_opacity(5.0);
  EXPECT_DOUBLE_EQ(mobject.opacity(), 1.0);
}

}  // namespace
