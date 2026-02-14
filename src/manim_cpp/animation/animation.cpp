#include "manim_cpp/animation/animation.hpp"

#include <utility>

namespace manim_cpp::animation {

void Animation::set_run_time_seconds(const double seconds) {
  run_time_seconds_ = seconds > 0.0 ? seconds : 1.0;
}

double Animation::run_time_seconds() const {
  return run_time_seconds_;
}

void Animation::set_rate_function(RateFunction function) {
  if (!function) {
    rate_function_ = [](double alpha) { return alpha; };
    return;
  }
  rate_function_ = std::move(function);
}

double Animation::apply_rate_function(const double alpha) const {
  return rate_function_(alpha);
}

void Animation::interpolate_with_rate(const double alpha) {
  interpolate(apply_rate_function(alpha));
}

}  // namespace manim_cpp::animation
