#pragma once

#include "manim_cpp/scene/moving_camera_scene.hpp"

namespace manim_cpp::scene {

class ZoomedScene : public MovingCameraScene {
 public:
  void set_zoom_factor(double zoom_factor);
  [[nodiscard]] double zoom_factor() const;

 private:
  double zoom_factor_ = 1.0;
};

}  // namespace manim_cpp::scene
