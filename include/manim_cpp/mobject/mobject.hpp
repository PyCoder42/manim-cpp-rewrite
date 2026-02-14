#pragma once

#include <memory>
#include <string>
#include <vector>

#include "manim_cpp/math/core.hpp"

namespace manim_cpp::mobject {

class Mobject {
 public:
  virtual ~Mobject() = default;
  virtual std::string debug_name() const { return "Mobject"; }

  void add(const std::shared_ptr<Mobject>& child);
  bool remove(const std::shared_ptr<Mobject>& child);
  void clear_submobjects();
  [[nodiscard]] const std::vector<std::shared_ptr<Mobject>>& submobjects() const;

  void move_to(const math::Vec3& point);
  void shift(const math::Vec3& delta);
  [[nodiscard]] const math::Vec3& center() const;

  void set_opacity(double opacity);
  [[nodiscard]] double opacity() const;

 private:
  std::vector<std::shared_ptr<Mobject>> submobjects_;
  math::Vec3 center_{0.0, 0.0, 0.0};
  double opacity_ = 1.0;
};

}  // namespace manim_cpp::mobject
