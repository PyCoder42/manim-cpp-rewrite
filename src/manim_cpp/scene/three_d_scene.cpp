#include "manim_cpp/scene/three_d_scene.hpp"

namespace manim_cpp::scene {

void ThreeDScene::set_camera_orientation(const double theta,
                                         const double phi,
                                         const double gamma) {
  theta_ = theta;
  phi_ = phi;
  gamma_ = gamma;
}

double ThreeDScene::theta() const {
  return theta_;
}

double ThreeDScene::phi() const {
  return phi_;
}

double ThreeDScene::gamma() const {
  return gamma_;
}

}  // namespace manim_cpp::scene
