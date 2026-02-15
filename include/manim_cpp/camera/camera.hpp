#pragma once

#include <cstddef>

namespace manim_cpp::camera {

class Camera {
 public:
  virtual ~Camera() = default;

  [[nodiscard]] virtual int frame_rate() const;
  void set_frame_rate(int frame_rate);

  [[nodiscard]] std::size_t pixel_width() const;
  [[nodiscard]] std::size_t pixel_height() const;
  void set_pixel_size(std::size_t width, std::size_t height);

  [[nodiscard]] double aspect_ratio() const;

 private:
  int frame_rate_ = 60;
  std::size_t pixel_width_ = 1920;
  std::size_t pixel_height_ = 1080;
};

}  // namespace manim_cpp::camera
