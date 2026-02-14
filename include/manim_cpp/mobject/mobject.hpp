#pragma once

#include <string>

namespace manim_cpp::mobject {

class Mobject {
 public:
  virtual ~Mobject() = default;
  virtual std::string debug_name() const { return "Mobject"; }
};

}  // namespace manim_cpp::mobject
