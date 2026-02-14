#pragma once

namespace manim_cpp::animation {

class Animation {
 public:
  virtual ~Animation() = default;
  virtual void begin() {}
  virtual void interpolate(double /*alpha*/) {}
  virtual void finish() {}
};

}  // namespace manim_cpp::animation
