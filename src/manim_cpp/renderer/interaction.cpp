#include "manim_cpp/renderer/interaction.hpp"

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <limits>

namespace manim_cpp::renderer {
namespace {

std::string lower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](const unsigned char ch) {
                   return static_cast<char>(std::tolower(ch));
                 });
  return value;
}

bool parse_int_strict(const std::string& value, int* output) {
  if (value.empty() || output == nullptr) {
    return false;
  }
  char* end = nullptr;
  errno = 0;
  const long parsed = std::strtol(value.c_str(), &end, 10);
  if (errno != 0 || end == value.c_str() || *end != '\0' ||
      parsed < std::numeric_limits<int>::min() ||
      parsed > std::numeric_limits<int>::max()) {
    return false;
  }
  *output = static_cast<int>(parsed);
  return true;
}

std::optional<std::pair<int, int>> parse_int_pair(const std::string& value) {
  const auto separator = value.find(',');
  if (separator == std::string::npos) {
    return std::nullopt;
  }
  const std::string left = value.substr(0, separator);
  const std::string right = value.substr(separator + 1);
  int first = 0;
  int second = 0;
  if (!parse_int_strict(left, &first) || !parse_int_strict(right, &second)) {
    return std::nullopt;
  }
  return std::make_pair(first, second);
}

}  // namespace

std::optional<WindowPosition> parse_window_position(std::string value) {
  value = lower(std::move(value));
  if (value == "ur") {
    return WindowPosition{.anchor = WindowAnchor::kUpperRight};
  }
  if (value == "ul") {
    return WindowPosition{.anchor = WindowAnchor::kUpperLeft};
  }
  if (value == "dr" || value == "lr") {
    return WindowPosition{.anchor = WindowAnchor::kLowerRight};
  }
  if (value == "dl" || value == "ll") {
    return WindowPosition{.anchor = WindowAnchor::kLowerLeft};
  }
  if (value == "center" || value == "c") {
    return WindowPosition{.anchor = WindowAnchor::kCenter};
  }

  const auto pair = parse_int_pair(value);
  if (!pair.has_value()) {
    return std::nullopt;
  }
  return WindowPosition{
      .anchor = WindowAnchor::kUpperLeft,
      .custom_coordinates = true,
      .x = pair->first,
      .y = pair->second,
  };
}

std::string to_string(const WindowPosition& value) {
  if (value.custom_coordinates) {
    return std::to_string(value.x) + "," + std::to_string(value.y);
  }
  switch (value.anchor) {
    case WindowAnchor::kUpperRight:
      return "UR";
    case WindowAnchor::kUpperLeft:
      return "UL";
    case WindowAnchor::kLowerRight:
      return "DR";
    case WindowAnchor::kLowerLeft:
      return "DL";
    case WindowAnchor::kCenter:
      return "CENTER";
  }
  return "UR";
}

std::optional<WindowSize> parse_window_size(std::string value) {
  value = lower(std::move(value));
  if (value == "default") {
    return WindowSize{};
  }

  const auto pair = parse_int_pair(value);
  if (!pair.has_value() || pair->first <= 0 || pair->second <= 0) {
    return std::nullopt;
  }

  return WindowSize{.use_default = false, .width = pair->first, .height = pair->second};
}

std::string to_string(const WindowSize& value) {
  if (value.use_default) {
    return "default";
  }
  return std::to_string(value.width) + "," + std::to_string(value.height);
}

InteractionSession::InteractionSession(InteractionConfig config)
    : config_(std::move(config)) {}

bool InteractionSession::should_open_window() const {
  return config_.interactive || config_.enable_gui || config_.fullscreen ||
         config_.force_window;
}

void InteractionSession::apply(const InteractionCommand command, const double step) {
  constexpr double kMinimumZoom = 0.05;
  constexpr double kMaximumZoom = 100.0;
  const double safe_step = step > 0.0 ? step : 1.0;

  switch (command) {
    case InteractionCommand::kPanLeft:
      camera_state_.pan_x -= safe_step;
      return;
    case InteractionCommand::kPanRight:
      camera_state_.pan_x += safe_step;
      return;
    case InteractionCommand::kPanUp:
      camera_state_.pan_y += safe_step;
      return;
    case InteractionCommand::kPanDown:
      camera_state_.pan_y -= safe_step;
      return;
    case InteractionCommand::kYawLeft:
      camera_state_.yaw -= safe_step;
      return;
    case InteractionCommand::kYawRight:
      camera_state_.yaw += safe_step;
      return;
    case InteractionCommand::kPitchUp:
      camera_state_.pitch += safe_step;
      return;
    case InteractionCommand::kPitchDown:
      camera_state_.pitch -= safe_step;
      return;
    case InteractionCommand::kZoomIn:
      camera_state_.zoom = std::min(kMaximumZoom, camera_state_.zoom * (1.0 + safe_step));
      return;
    case InteractionCommand::kZoomOut:
      camera_state_.zoom =
          std::max(kMinimumZoom, camera_state_.zoom / (1.0 + safe_step));
      return;
    case InteractionCommand::kResetCamera:
      reset_camera();
      return;
  }
}

void InteractionSession::reset_camera() { camera_state_ = CameraInteractionState{}; }

}  // namespace manim_cpp::renderer
