#include "manim_cpp/scene/scene_file_writer.hpp"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace manim_cpp::scene {
namespace {

std::string format_srt_timestamp(const double seconds) {
  const auto total_milliseconds = static_cast<long long>(std::llround(seconds * 1000.0));
  const auto hours = total_milliseconds / 3600000;
  const auto minutes = (total_milliseconds % 3600000) / 60000;
  const auto secs = (total_milliseconds % 60000) / 1000;
  const auto millis = total_milliseconds % 1000;

  std::ostringstream stream;
  stream << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2) << minutes << ":"
         << std::setw(2) << secs << "," << std::setw(3) << millis;
  return stream.str();
}

}  // namespace

SceneFileWriter::SceneFileWriter(std::string scene_name)
    : scene_name_(std::move(scene_name)) {
  sections_.emplace_back("autocreated", false);
}

void SceneFileWriter::begin_animation(bool /*write_frames*/) {}

void SceneFileWriter::end_animation(bool /*write_frames*/) {}

void SceneFileWriter::add_partial_movie_file(const std::string& path) {
  if (sections_.empty()) {
    sections_.emplace_back("autocreated", false);
  }
  sections_.back().add_partial_movie_file(path);
}

void SceneFileWriter::add_subcaption(const std::string& content,
                                     const double start_seconds,
                                     const double end_seconds) {
  if (end_seconds <= start_seconds) {
    return;
  }
  subcaptions_.push_back(Subcaption{.content = content,
                                    .start_seconds = start_seconds,
                                    .end_seconds = end_seconds});
}

bool SceneFileWriter::write_subcaptions_srt(const std::filesystem::path& output_path) const {
  std::ofstream output(output_path);
  if (!output.is_open()) {
    return false;
  }

  std::size_t index = 1;
  for (const auto& subcaption : subcaptions_) {
    output << index << "\n";
    output << format_srt_timestamp(subcaption.start_seconds) << " --> "
           << format_srt_timestamp(subcaption.end_seconds) << "\n";
    output << subcaption.content << "\n\n";
    ++index;
  }

  return output.good();
}

}  // namespace manim_cpp::scene
