#include "manim_cpp/camera/camera.hpp"

namespace manim_cpp::camera {

int Camera::frame_rate() const {
  return frame_rate_;
}

void Camera::set_frame_rate(const int frame_rate) {
  frame_rate_ = frame_rate > 0 ? frame_rate : 60;
}

std::size_t Camera::pixel_width() const {
  return pixel_width_;
}

std::size_t Camera::pixel_height() const {
  return pixel_height_;
}

void Camera::set_pixel_size(const std::size_t width, const std::size_t height) {
  if (width == 0 || height == 0) {
    return;
  }
  pixel_width_ = width;
  pixel_height_ = height;
}

double Camera::aspect_ratio() const {
  return static_cast<double>(pixel_width_) / static_cast<double>(pixel_height_);
}

}  // namespace manim_cpp::camera
