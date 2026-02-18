#pragma once

#include <optional>
#include <string>

namespace manim_cpp::renderer {

enum class WindowAnchor {
  kUpperRight,
  kUpperLeft,
  kLowerRight,
  kLowerLeft,
  kCenter,
};

struct WindowPosition {
  WindowAnchor anchor = WindowAnchor::kUpperRight;
  bool custom_coordinates = false;
  int x = 0;
  int y = 0;
};

struct WindowSize {
  bool use_default = true;
  int width = 0;
  int height = 0;
};

std::optional<WindowPosition> parse_window_position(std::string value);
std::string to_string(const WindowPosition& value);

std::optional<WindowSize> parse_window_size(std::string value);
std::string to_string(const WindowSize& value);

struct InteractionConfig {
  bool watch = false;
  bool interactive = false;
  bool enable_gui = false;
  bool fullscreen = false;
  bool force_window = false;
  int window_monitor = 0;
  WindowPosition window_position{};
  WindowSize window_size{};
};

struct CameraInteractionState {
  double pan_x = 0.0;
  double pan_y = 0.0;
  double yaw = 0.0;
  double pitch = 0.0;
  double zoom = 1.0;
};

enum class InteractionCommand {
  kPanLeft,
  kPanRight,
  kPanUp,
  kPanDown,
  kYawLeft,
  kYawRight,
  kPitchUp,
  kPitchDown,
  kZoomIn,
  kZoomOut,
  kResetCamera,
};

struct ParsedInteractionCommand {
  InteractionCommand command = InteractionCommand::kResetCamera;
  double step = 1.0;
};

std::optional<ParsedInteractionCommand> parse_interaction_command_step(std::string token);

class InteractionSession {
 public:
  explicit InteractionSession(InteractionConfig config = {});

  const InteractionConfig& config() const { return config_; }
  const CameraInteractionState& camera_state() const { return camera_state_; }

  bool should_open_window() const;
  void apply(InteractionCommand command, double step = 1.0);
  void reset_camera();

 private:
  InteractionConfig config_;
  CameraInteractionState camera_state_;
};

}  // namespace manim_cpp::renderer
