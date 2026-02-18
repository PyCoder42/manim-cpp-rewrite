#include "manim_cpp/renderer/interaction.hpp"

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cmath>
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

bool parse_double_strict(const std::string& value, double* output) {
  if (value.empty() || output == nullptr) {
    return false;
  }
  char* end = nullptr;
  errno = 0;
  const double parsed = std::strtod(value.c_str(), &end);
  if (errno != 0 || end == value.c_str() || *end != '\0' || !std::isfinite(parsed)) {
    return false;
  }
  *output = parsed;
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

std::optional<ParsedInteractionCommand> parse_interaction_command_step(std::string token) {
  token = lower(std::move(token));
  if (token.empty()) {
    return std::nullopt;
  }

  std::string command_name = token;
  double step = 1.0;
  const auto separator = token.find(':');
  if (separator != std::string::npos) {
    if (token.find(':', separator + 1) != std::string::npos) {
      return std::nullopt;
    }
    command_name = token.substr(0, separator);
    const std::string raw_step = token.substr(separator + 1);
    if (!parse_double_strict(raw_step, &step) || step <= 0.0) {
      return std::nullopt;
    }
  }

  std::optional<InteractionCommand> command = std::nullopt;
  if (command_name == "pan_left") {
    command = InteractionCommand::kPanLeft;
  } else if (command_name == "pan_right") {
    command = InteractionCommand::kPanRight;
  } else if (command_name == "pan_up") {
    command = InteractionCommand::kPanUp;
  } else if (command_name == "pan_down") {
    command = InteractionCommand::kPanDown;
  } else if (command_name == "yaw_left") {
    command = InteractionCommand::kYawLeft;
  } else if (command_name == "yaw_right") {
    command = InteractionCommand::kYawRight;
  } else if (command_name == "pitch_up") {
    command = InteractionCommand::kPitchUp;
  } else if (command_name == "pitch_down") {
    command = InteractionCommand::kPitchDown;
  } else if (command_name == "zoom_in") {
    command = InteractionCommand::kZoomIn;
  } else if (command_name == "zoom_out") {
    command = InteractionCommand::kZoomOut;
  } else if (command_name == "reset_camera") {
    command = InteractionCommand::kResetCamera;
    step = 1.0;
  } else {
    return std::nullopt;
  }

  return ParsedInteractionCommand{.command = command.value(), .step = step};
}

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
