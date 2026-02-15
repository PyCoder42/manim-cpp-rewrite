#pragma once

#include <memory>

#include "manim_cpp/animation/animation.hpp"
#include "manim_cpp/math/core.hpp"

namespace manim_cpp::mobject {
class Mobject;
}

namespace manim_cpp::animation {

class MoveToAnimation : public Animation {
 public:
  MoveToAnimation(std::shared_ptr<mobject::Mobject> target, math::Vec3 destination);

  [[nodiscard]] std::shared_ptr<mobject::Mobject> target() const;
  [[nodiscard]] const math::Vec3& destination() const;

  void begin() override;
  void interpolate(double alpha) override;

 private:
  std::shared_ptr<mobject::Mobject> target_;
  math::Vec3 destination_;
  math::Vec3 start_{0.0, 0.0, 0.0};
};

class ShiftAnimation : public Animation {
 public:
  ShiftAnimation(std::shared_ptr<mobject::Mobject> target, math::Vec3 delta);

  [[nodiscard]] std::shared_ptr<mobject::Mobject> target() const;
  [[nodiscard]] const math::Vec3& delta() const;

  void begin() override;
  void interpolate(double alpha) override;

 private:
  std::shared_ptr<mobject::Mobject> target_;
  math::Vec3 delta_;
  math::Vec3 start_{0.0, 0.0, 0.0};
};

class FadeToOpacityAnimation : public Animation {
 public:
  FadeToOpacityAnimation(std::shared_ptr<mobject::Mobject> target, double target_opacity);

  [[nodiscard]] std::shared_ptr<mobject::Mobject> target() const;
  [[nodiscard]] double target_opacity() const;

  void begin() override;
  void interpolate(double alpha) override;

 private:
  std::shared_ptr<mobject::Mobject> target_;
  double target_opacity_ = 1.0;
  double start_opacity_ = 1.0;
};

}  // namespace manim_cpp::animation
