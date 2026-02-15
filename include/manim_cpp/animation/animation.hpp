#pragma once

#include <functional>

namespace manim_cpp::animation {

using RateFunction = std::function<double(double)>;

class Animation {
 public:
  virtual ~Animation() = default;
  virtual void begin() {}
  virtual void interpolate(double /*alpha*/) {}
  virtual void finish() {}

  void set_run_time_seconds(double seconds);
  [[nodiscard]] double run_time_seconds() const;

  void set_rate_function(RateFunction function);
  [[nodiscard]] double apply_rate_function(double alpha) const;
  void interpolate_with_rate(double alpha);

 private:
  double run_time_seconds_ = 1.0;
  RateFunction rate_function_ = [](double alpha) { return alpha; };
};

}  // namespace manim_cpp::animation
