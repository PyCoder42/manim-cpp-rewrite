#include "manim_cpp/scene/zoomed_scene.hpp"

namespace manim_cpp::scene {

void ZoomedScene::set_zoom_factor(const double zoom_factor) {
  if (zoom_factor <= 0.0) {
    return;
  }
  zoom_factor_ = zoom_factor;
}

double ZoomedScene::zoom_factor() const {
  return zoom_factor_;
}

}  // namespace manim_cpp::scene
