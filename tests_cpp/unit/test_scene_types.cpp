#include <gtest/gtest.h>

#include "manim_cpp/scene/moving_camera_scene.hpp"
#include "manim_cpp/scene/three_d_scene.hpp"
#include "manim_cpp/scene/zoomed_scene.hpp"

namespace {

class MovingCameraSceneFixture final : public manim_cpp::scene::MovingCameraScene {
 public:
  void construct() override {}
};

class ThreeDSceneFixture final : public manim_cpp::scene::ThreeDScene {
 public:
  void construct() override {}
};

class ZoomedSceneFixture final : public manim_cpp::scene::ZoomedScene {
 public:
  void construct() override {}
};

TEST(SceneTypes, MovingCameraSceneExposesMutableCameraSettings) {
  MovingCameraSceneFixture scene;
  EXPECT_EQ(scene.camera().frame_rate(), 60);
  scene.camera().set_frame_rate(24);
  scene.camera().set_pixel_size(640, 360);
  EXPECT_EQ(scene.camera().frame_rate(), 24);
  EXPECT_EQ(scene.camera().pixel_width(), static_cast<size_t>(640));
  EXPECT_EQ(scene.camera().pixel_height(), static_cast<size_t>(360));
}

TEST(SceneTypes, ThreeDSceneTracksCameraOrientationValues) {
  ThreeDSceneFixture scene;
  EXPECT_DOUBLE_EQ(scene.theta(), 0.0);
  EXPECT_DOUBLE_EQ(scene.phi(), 0.0);
  EXPECT_DOUBLE_EQ(scene.gamma(), 0.0);

  scene.set_camera_orientation(1.0, 0.5, -0.25);
  EXPECT_DOUBLE_EQ(scene.theta(), 1.0);
  EXPECT_DOUBLE_EQ(scene.phi(), 0.5);
  EXPECT_DOUBLE_EQ(scene.gamma(), -0.25);
}

TEST(SceneTypes, ZoomedSceneMaintainsPositiveZoomFactor) {
  ZoomedSceneFixture scene;
  EXPECT_DOUBLE_EQ(scene.zoom_factor(), 1.0);
  scene.set_zoom_factor(2.5);
  EXPECT_DOUBLE_EQ(scene.zoom_factor(), 2.5);

  scene.set_zoom_factor(0.0);
  EXPECT_DOUBLE_EQ(scene.zoom_factor(), 2.5);
}

}  // namespace
