#include <gtest/gtest.h>

#include "manim_cpp/renderer/interaction.hpp"

TEST(Interaction, ParsesWindowPositionAnchorsAndCoordinates) {
  const auto upper_right = manim_cpp::renderer::parse_window_position("UR");
  ASSERT_TRUE(upper_right.has_value());
  EXPECT_FALSE(upper_right->custom_coordinates);
  EXPECT_EQ(manim_cpp::renderer::to_string(upper_right.value()), std::string("UR"));

  const auto center = manim_cpp::renderer::parse_window_position("center");
  ASSERT_TRUE(center.has_value());
  EXPECT_EQ(manim_cpp::renderer::to_string(center.value()), std::string("CENTER"));

  const auto custom = manim_cpp::renderer::parse_window_position("960,540");
  ASSERT_TRUE(custom.has_value());
  EXPECT_TRUE(custom->custom_coordinates);
  EXPECT_EQ(custom->x, 960);
  EXPECT_EQ(custom->y, 540);
  EXPECT_EQ(manim_cpp::renderer::to_string(custom.value()), std::string("960,540"));
}

TEST(Interaction, RejectsInvalidWindowPositionValues) {
  EXPECT_FALSE(manim_cpp::renderer::parse_window_position("").has_value());
  EXPECT_FALSE(manim_cpp::renderer::parse_window_position("north-east").has_value());
  EXPECT_FALSE(manim_cpp::renderer::parse_window_position("1200x800").has_value());
  EXPECT_FALSE(manim_cpp::renderer::parse_window_position("1,2,3").has_value());
}

TEST(Interaction, ParsesWindowSizeDefaultAndExplicitDimensions) {
  const auto default_size = manim_cpp::renderer::parse_window_size("default");
  ASSERT_TRUE(default_size.has_value());
  EXPECT_TRUE(default_size->use_default);
  EXPECT_EQ(manim_cpp::renderer::to_string(default_size.value()),
            std::string("default"));

  const auto explicit_size = manim_cpp::renderer::parse_window_size("1280,720");
  ASSERT_TRUE(explicit_size.has_value());
  EXPECT_FALSE(explicit_size->use_default);
  EXPECT_EQ(explicit_size->width, 1280);
  EXPECT_EQ(explicit_size->height, 720);
  EXPECT_EQ(manim_cpp::renderer::to_string(explicit_size.value()),
            std::string("1280,720"));
}

TEST(Interaction, RejectsInvalidWindowSizeValues) {
  EXPECT_FALSE(manim_cpp::renderer::parse_window_size("0,720").has_value());
  EXPECT_FALSE(manim_cpp::renderer::parse_window_size("-1,720").has_value());
  EXPECT_FALSE(manim_cpp::renderer::parse_window_size("720").has_value());
  EXPECT_FALSE(manim_cpp::renderer::parse_window_size("1280x720").has_value());
}

TEST(Interaction, SessionWindowOpenSignalFollowsInteractionFlags) {
  manim_cpp::renderer::InteractionSession default_session;
  EXPECT_FALSE(default_session.should_open_window());

  manim_cpp::renderer::InteractionConfig config;
  config.interactive = true;
  manim_cpp::renderer::InteractionSession interactive_session(config);
  EXPECT_TRUE(interactive_session.should_open_window());

  config = manim_cpp::renderer::InteractionConfig{};
  config.enable_gui = true;
  manim_cpp::renderer::InteractionSession gui_session(config);
  EXPECT_TRUE(gui_session.should_open_window());
}

TEST(Interaction, SessionAppliesCameraCommandsDeterministically) {
  manim_cpp::renderer::InteractionSession session;

  session.apply(manim_cpp::renderer::InteractionCommand::kPanRight, 2.0);
  session.apply(manim_cpp::renderer::InteractionCommand::kPanUp, 3.0);
  session.apply(manim_cpp::renderer::InteractionCommand::kYawLeft, 0.5);
  session.apply(manim_cpp::renderer::InteractionCommand::kPitchDown, 0.25);
  session.apply(manim_cpp::renderer::InteractionCommand::kZoomIn, 0.5);

  const auto state = session.camera_state();
  EXPECT_DOUBLE_EQ(state.pan_x, 2.0);
  EXPECT_DOUBLE_EQ(state.pan_y, 3.0);
  EXPECT_DOUBLE_EQ(state.yaw, -0.5);
  EXPECT_DOUBLE_EQ(state.pitch, -0.25);
  EXPECT_DOUBLE_EQ(state.zoom, 1.5);

  session.apply(manim_cpp::renderer::InteractionCommand::kResetCamera);
  const auto reset = session.camera_state();
  EXPECT_DOUBLE_EQ(reset.pan_x, 0.0);
  EXPECT_DOUBLE_EQ(reset.pan_y, 0.0);
  EXPECT_DOUBLE_EQ(reset.yaw, 0.0);
  EXPECT_DOUBLE_EQ(reset.pitch, 0.0);
  EXPECT_DOUBLE_EQ(reset.zoom, 1.0);
}
