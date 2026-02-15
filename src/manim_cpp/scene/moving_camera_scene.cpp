#include "manim_cpp/scene/moving_camera_scene.hpp"

namespace manim_cpp::scene {

camera::Camera& MovingCameraScene::camera() {
  return camera_;
}

const camera::Camera& MovingCameraScene::camera() const {
  return camera_;
}

}  // namespace manim_cpp::scene
