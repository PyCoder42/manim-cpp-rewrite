#include "manim_cpp/mobject/value_tracker.hpp"

namespace manim_cpp::mobject {

ValueTracker::ValueTracker(const double value) : value_(value) {}

double ValueTracker::value() const {
  return value_;
}

void ValueTracker::set_value(const double value) {
  value_ = value;
}

void ValueTracker::increment_value(const double delta) {
  value_ += delta;
}

}  // namespace manim_cpp::mobject
