#include <gtest/gtest.h>

#include "manim_cpp/mobject/value_tracker.hpp"

namespace {

using manim_cpp::mobject::ValueTracker;

TEST(ValueTracker, TracksValueThroughSetAndIncrement) {
  ValueTracker tracker;
  EXPECT_DOUBLE_EQ(tracker.value(), 0.0);

  tracker.set_value(2.5);
  EXPECT_DOUBLE_EQ(tracker.value(), 2.5);

  tracker.increment_value(-1.25);
  EXPECT_DOUBLE_EQ(tracker.value(), 1.25);
}

TEST(ValueTracker, SupportsDeterministicHighPrecisionValues) {
  ValueTracker tracker(1.0 / 3.0);
  tracker.increment_value(2.0 / 3.0);
  EXPECT_NEAR(tracker.value(), 1.0, 1e-12);
}

}  // namespace
