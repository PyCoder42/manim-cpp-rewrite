#pragma once

#include <string>

namespace manim_cpp::scene {

class Scene {
 public:
  virtual ~Scene() = default;

  virtual void setup() {}
  virtual void construct() = 0;
  virtual void tear_down() {}

  virtual std::string scene_name() const { return "Scene"; }
};

}  // namespace manim_cpp::scene
