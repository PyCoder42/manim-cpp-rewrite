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
std::string codec_hint_for_format(MediaFormat format);

}  // namespace manim_cpp::scene
