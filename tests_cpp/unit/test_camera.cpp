#include <gtest/gtest.h>

#include "manim_cpp/camera/camera.hpp"

namespace {

using manim_cpp::camera::Camera;

TEST(Camera, ExposesDefaultFrameRateAndPixelSize) {
  Camera camera;
  EXPECT_EQ(camera.frame_rate(), 60);
  EXPECT_EQ(camera.pixel_width(), static_cast<size_t>(1920));
  EXPECT_EQ(camera.pixel_height(), static_cast<size_t>(1080));
  EXPECT_NEAR(camera.aspect_ratio(), 16.0 / 9.0, 1e-12);
}

TEST(Camera, ValidatesFrameRateAndPixelSizeUpdates) {
  Camera camera;
  camera.set_frame_rate(30);
  EXPECT_EQ(camera.frame_rate(), 30);

  camera.set_frame_rate(0);
  EXPECT_EQ(camera.frame_rate(), 60);

  camera.set_pixel_size(3840, 2160);
  EXPECT_EQ(camera.pixel_width(), static_cast<size_t>(3840));
  EXPECT_EQ(camera.pixel_height(), static_cast<size_t>(2160));
  EXPECT_NEAR(camera.aspect_ratio(), 16.0 / 9.0, 1e-12);

  camera.set_pixel_size(0, 100);
  EXPECT_EQ(camera.pixel_width(), static_cast<size_t>(3840));
  EXPECT_EQ(camera.pixel_height(), static_cast<size_t>(2160));
}

}  // namespace
