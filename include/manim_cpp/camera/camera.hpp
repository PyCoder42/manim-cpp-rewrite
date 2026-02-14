#pragma once

namespace manim_cpp::camera {

class Camera {
 public:
  virtual ~Camera() = default;
  virtual int frame_rate() const { return 60; }
};

}  // namespace manim_cpp::camera
