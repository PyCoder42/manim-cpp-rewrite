#pragma once

#include "manim_cpp/camera/camera.hpp"
#include "manim_cpp/scene/scene.hpp"

namespace manim_cpp::scene {

class MovingCameraScene : public Scene {
 public:
  camera::Camera& camera();
  const camera::Camera& camera() const;

 private:
  class OwnedCamera final : public camera::Camera {};
  OwnedCamera camera_;
};

}  // namespace manim_cpp::scene
