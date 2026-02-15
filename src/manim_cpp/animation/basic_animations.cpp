#include "manim_cpp/animation/basic_animations.hpp"

#include "manim_cpp/mobject/mobject.hpp"

namespace manim_cpp::animation {

namespace {

math::Vec3 lerp_vec3(const math::Vec3& a, const math::Vec3& b, const double alpha) {
  return math::Vec3{
      a[0] + ((b[0] - a[0]) * alpha),
      a[1] + ((b[1] - a[1]) * alpha),
      a[2] + ((b[2] - a[2]) * alpha),
  };
}

double lerp_double(const double a, const double b, const double alpha) {
  return a + ((b - a) * alpha);
}

}  // namespace

MoveToAnimation::MoveToAnimation(std::shared_ptr<mobject::Mobject> target,
                                 const math::Vec3 destination)
    : target_(std::move(target)), destination_(destination) {}

std::shared_ptr<mobject::Mobject> MoveToAnimation::target() const {
  return target_;
}

const math::Vec3& MoveToAnimation::destination() const {
  return destination_;
}

void MoveToAnimation::begin() {
  if (!target_) {
    return;
  }
  start_ = target_->center();
}

void MoveToAnimation::interpolate(const double alpha) {
  if (!target_) {
    return;
  }
  target_->move_to(lerp_vec3(start_, destination_, alpha));
}

ShiftAnimation::ShiftAnimation(std::shared_ptr<mobject::Mobject> target, const math::Vec3 delta)
    : target_(std::move(target)), delta_(delta) {}

std::shared_ptr<mobject::Mobject> ShiftAnimation::target() const {
  return target_;
}

const math::Vec3& ShiftAnimation::delta() const {
  return delta_;
}

void ShiftAnimation::begin() {
  if (!target_) {
    return;
  }
  start_ = target_->center();
}

void ShiftAnimation::interpolate(const double alpha) {
  if (!target_) {
    return;
  }
  const math::Vec3 destination{
      start_[0] + delta_[0],
      start_[1] + delta_[1],
      start_[2] + delta_[2],
  };
  target_->move_to(lerp_vec3(start_, destination, alpha));
}

FadeToOpacityAnimation::FadeToOpacityAnimation(std::shared_ptr<mobject::Mobject> target,
                                               const double target_opacity)
    : target_(std::move(target)), target_opacity_(target_opacity) {}

std::shared_ptr<mobject::Mobject> FadeToOpacityAnimation::target() const {
  return target_;
}

double FadeToOpacityAnimation::target_opacity() const {
  return target_opacity_;
}

void FadeToOpacityAnimation::begin() {
  if (!target_) {
    return;
  }
  start_opacity_ = target_->opacity();
}

void FadeToOpacityAnimation::interpolate(const double alpha) {
  if (!target_) {
    return;
  }
  target_->set_opacity(lerp_double(start_opacity_, target_opacity_, alpha));
}

}  // namespace manim_cpp::animation
