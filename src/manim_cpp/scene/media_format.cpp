#include "manim_cpp/scene/media_format.hpp"

#include <algorithm>
#include <cctype>
#include <utility>

namespace manim_cpp::scene {
namespace {

std::string lower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](const unsigned char ch) {
                   return static_cast<char>(std::tolower(ch));
                 });
  return value;
}

}  // namespace

std::string to_string(const MediaFormat format) {
  switch (format) {
    case MediaFormat::kPng:
      return "png";
    case MediaFormat::kGif:
      return "gif";
    case MediaFormat::kMp4:
      return "mp4";
    case MediaFormat::kWebm:
      return "webm";
    case MediaFormat::kMov:
      return "mov";
  }
  return "unknown";
}

std::optional<MediaFormat> parse_media_format(std::string format_name) {
  format_name = lower(std::move(format_name));
  if (format_name == "png") {
    return MediaFormat::kPng;
  }
  if (format_name == "gif") {
    return MediaFormat::kGif;
  }
  if (format_name == "mp4") {
    return MediaFormat::kMp4;
  }
  if (format_name == "webm") {
    return MediaFormat::kWebm;
  }
  if (format_name == "mov") {
    return MediaFormat::kMov;
  }
  return std::nullopt;
}

}  // namespace manim_cpp::scene
