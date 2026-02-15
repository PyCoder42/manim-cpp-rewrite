#pragma once

#include <string>

#include "manim_cpp/mobject/mobject.hpp"

namespace manim_cpp::mobject {

class ValueTracker : public Mobject {
 public:
  explicit ValueTracker(double value = 0.0);

  [[nodiscard]] std::string debug_name() const override { return "ValueTracker"; }
  [[nodiscard]] double value() const;
  void set_value(double value);
  void increment_value(double delta);

 private:
  double value_ = 0.0;
};

}  // namespace manim_cpp::mobject
