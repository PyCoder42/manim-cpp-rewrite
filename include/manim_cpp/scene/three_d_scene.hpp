#pragma once

#include "manim_cpp/scene/moving_camera_scene.hpp"

namespace manim_cpp::scene {

class ThreeDScene : public MovingCameraScene {
 public:
  void set_camera_orientation(double theta, double phi, double gamma = 0.0);

  [[nodiscard]] double theta() const;
  [[nodiscard]] double phi() const;
  [[nodiscard]] double gamma() const;

 private:
  double theta_ = 0.0;
  double phi_ = 0.0;
  double gamma_ = 0.0;
};

}  // namespace manim_cpp::scene
