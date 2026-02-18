#pragma once

#include <cstddef>
#include <vector>

#include "manim_cpp/animation/animation.hpp"

namespace manim_cpp::animation {

class ParallelAnimation : public Animation {
 public:
  explicit ParallelAnimation(std::vector<Animation*> animations);

  void begin() override;
  void interpolate(double alpha) override;
  void finish() override;

 private:
  std::vector<Animation*> animations_;
};

class SuccessionAnimation : public Animation {
 public:
  explicit SuccessionAnimation(std::vector<Animation*> animations);

  void begin() override;
  void interpolate(double alpha) override;
  void finish() override;

 private:
  std::vector<Animation*> animations_;
  std::vector<double> start_times_;
  std::vector<double> durations_;
  double total_duration_ = 1.0;
};

class LaggedStartAnimation : public Animation {
 public:
  LaggedStartAnimation(std::vector<Animation*> animations, double lag_ratio = 0.0);

  void begin() override;
  void interpolate(double alpha) override;
  void finish() override;

 private:
  std::vector<Animation*> animations_;
  std::vector<double> start_times_;
  std::vector<double> durations_;
  double lag_ratio_ = 0.0;
  double total_duration_ = 1.0;
};

}  // namespace manim_cpp::animation
