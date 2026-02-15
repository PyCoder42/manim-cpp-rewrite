#pragma once

#include <optional>
#include <string>

namespace manim_cpp::scene {

enum class MediaFormat {
  kPng,
  kGif,
  kMp4,
  kWebm,
  kMov,
};

std::string to_string(MediaFormat format);
std::optional<MediaFormat> parse_media_format(std::string format_name);

}  // namespace manim_cpp::scene
