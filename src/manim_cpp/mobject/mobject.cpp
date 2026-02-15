#include "manim_cpp/mobject/mobject.hpp"

#include <algorithm>

namespace manim_cpp::mobject {

void Mobject::add(const std::shared_ptr<Mobject>& child) {
  if (child == nullptr || child.get() == this) {
    return;
  }
  if (std::find(submobjects_.begin(), submobjects_.end(), child) !=
      submobjects_.end()) {
    return;
  }
  submobjects_.push_back(child);
}

bool Mobject::remove(const std::shared_ptr<Mobject>& child) {
  const auto it = std::find(submobjects_.begin(), submobjects_.end(), child);
  if (it == submobjects_.end()) {
    return false;
  }
  submobjects_.erase(it);
  return true;
}

void Mobject::clear_submobjects() { submobjects_.clear(); }

const std::vector<std::shared_ptr<Mobject>>& Mobject::submobjects() const {
  return submobjects_;
}

void Mobject::move_to(const math::Vec3& point) { center_ = point; }

void Mobject::shift(const math::Vec3& delta) {
  center_[0] += delta[0];
  center_[1] += delta[1];
  center_[2] += delta[2];
}

const math::Vec3& Mobject::center() const { return center_; }

void Mobject::set_opacity(const double opacity) {
  opacity_ = std::clamp(opacity, 0.0, 1.0);
}

double Mobject::opacity() const { return opacity_; }

}  // namespace manim_cpp::mobject
