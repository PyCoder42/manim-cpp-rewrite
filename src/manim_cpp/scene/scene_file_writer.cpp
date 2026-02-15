#include "manim_cpp/scene/scene_file_writer.hpp"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_map>

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

std::string escape_json(const std::string& value) {
  std::string escaped;
  escaped.reserve(value.size());
  for (const char ch : value) {
    switch (ch) {
      case '"':
        escaped += "\\\"";
        break;
      case '\\':
        escaped += "\\\\";
        break;
      case '\n':
        escaped += "\\n";
        break;
      case '\r':
        escaped += "\\r";
        break;
      case '\t':
        escaped += "\\t";
        break;
      default:
        escaped.push_back(ch);
        break;
    }
  }
  return escaped;
}

std::string codec_hint_for_format(const std::string& format) {
  if (format == "png") {
    return "image/png-sequence";
  }
  if (format == "gif") {
    return "gif";
  }
  if (format == "mp4") {
    return "h264+aac";
  }
  if (format == "webm") {
    return "vp9+opus";
  }
  if (format == "mov") {
    return "prores+pcm";
  }
  return "unknown";
}

}  // namespace

SceneFileWriter::SceneFileWriter(std::string scene_name)
    : scene_name_(std::move(scene_name)) {
  sections_.emplace_back("autocreated", false);
}

void SceneFileWriter::begin_section(const std::string& name,
                                    const bool skip_animations) {
  sections_.emplace_back(name, skip_animations);
}

void SceneFileWriter::begin_animation(const bool write_frames) {
  animation_active_ = true;
  active_animation_writes_frames_ = write_frames;
}

void SceneFileWriter::end_animation(const bool write_frames) {
  if (!animation_active_) {
    return;
  }

  const bool section_skips_animations =
      !sections_.empty() && sections_.back().skip_animations();
  const bool should_write_partial_movie = active_animation_writes_frames_ &&
                                          write_frames &&
                                          !section_skips_animations;
  if (should_write_partial_movie) {
    ++rendered_animation_count_;
    add_partial_movie_file(make_partial_movie_file_name(rendered_animation_count_));
  }

  animation_active_ = false;
  active_animation_writes_frames_ = false;
}

void SceneFileWriter::add_partial_movie_file(const std::string& path) {
  if (sections_.empty()) {
    sections_.emplace_back("autocreated", false);
  }
  sections_.back().add_partial_movie_file(path);
}

void SceneFileWriter::set_section_timeline(const double start_seconds,
                                           const double end_seconds) {
  if (sections_.empty()) {
    sections_.emplace_back("autocreated", false);
  }
  sections_.back().set_timeline(start_seconds, end_seconds);
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

void SceneFileWriter::add_audio_segment(const std::string& path,
                                        const double start_seconds,
                                        const double gain_db) {
  if (path.empty() || start_seconds < 0.0) {
    return;
  }
  audio_segments_.push_back(AudioSegment{
      .path = path,
      .start_seconds = start_seconds,
      .gain_db = gain_db,
  });
}

void SceneFileWriter::set_render_summary(
    const std::size_t frame_count,
    const std::size_t pixel_width,
    const std::size_t pixel_height,
    const double frame_rate,
    const std::string& format,
    std::optional<std::filesystem::path> output_file) {
  if (pixel_width == 0 || pixel_height == 0 || frame_rate <= 0.0 || format.empty()) {
    return;
  }

  render_summary_.frame_count = frame_count;
  render_summary_.pixel_width = pixel_width;
  render_summary_.pixel_height = pixel_height;
  render_summary_.frame_rate = frame_rate;
  render_summary_.format = format;
  render_summary_.codec_hint = codec_hint_for_format(format);
  render_summary_.duration_seconds =
      static_cast<double>(frame_count) / frame_rate;
  render_summary_.output_file = std::move(output_file);
}

std::optional<SceneOutputPaths> SceneFileWriter::resolve_output_paths(
    const manim_cpp::config::ManimConfig& config,
    const std::string& module_name,
    const std::string& quality) const {
  const std::unordered_map<std::string, std::string> substitutions = {
      {"module_name", module_name},
      {"quality", quality},
      {"scene_name", scene_name_},
  };

  auto images_dir = config.resolve_path("CLI", "images_dir", substitutions);
  auto video_dir = config.resolve_path("CLI", "video_dir", substitutions);
  auto partial_movie_dir =
      config.resolve_path("CLI", "partial_movie_dir", substitutions);
  if (!images_dir.has_value() || !video_dir.has_value() ||
      !partial_movie_dir.has_value()) {
    return std::nullopt;
  }

  return SceneOutputPaths{
      .images_dir = images_dir.value(),
      .video_dir = video_dir.value(),
      .partial_movie_dir = partial_movie_dir.value(),
  };
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

bool SceneFileWriter::write_media_manifest(
    const std::filesystem::path& output_path) const {
  std::ofstream output(output_path);
  if (!output.is_open()) {
    return false;
  }

  output << "{";
  output << "\"scene\":\"" << escape_json(scene_name_) << "\",";

  output << "\"sections\":[";
  for (std::size_t i = 0; i < sections_.size(); ++i) {
    const auto& section = sections_[i];
    if (i > 0) {
      output << ",";
    }
    output << "{";
    output << "\"name\":\"" << escape_json(section.name()) << "\",";
    output << "\"skip_animations\":"
           << (section.skip_animations() ? "true" : "false") << ",";
    if (section.start_seconds().has_value()) {
      output << "\"start_seconds\":" << section.start_seconds().value() << ",";
    } else {
      output << "\"start_seconds\":null,";
    }
    if (section.end_seconds().has_value()) {
      output << "\"end_seconds\":" << section.end_seconds().value() << ",";
    } else {
      output << "\"end_seconds\":null,";
    }
    output << "\"partial_movie_files\":[";
    for (std::size_t j = 0; j < section.partial_movie_files().size(); ++j) {
      if (j > 0) {
        output << ",";
      }
      output << "\"" << escape_json(section.partial_movie_files()[j]) << "\"";
    }
    output << "]";
    output << "}";
  }
  output << "],";

  output << "\"subcaptions\":[";
  for (std::size_t i = 0; i < subcaptions_.size(); ++i) {
    const auto& subcaption = subcaptions_[i];
    if (i > 0) {
      output << ",";
    }
    output << "{";
    output << "\"content\":\"" << escape_json(subcaption.content) << "\",";
    output << "\"start_seconds\":" << subcaption.start_seconds << ",";
    output << "\"end_seconds\":" << subcaption.end_seconds;
    output << "}";
  }
  output << "],";

  output << "\"audio_segments\":[";
  for (std::size_t i = 0; i < audio_segments_.size(); ++i) {
    const auto& segment = audio_segments_[i];
    if (i > 0) {
      output << ",";
    }
    output << "{";
    output << "\"path\":\"" << escape_json(segment.path) << "\",";
    output << "\"start_seconds\":" << segment.start_seconds << ",";
    output << "\"gain_db\":" << segment.gain_db;
    output << "}";
  }
  output << "],";

  output << "\"render_summary\":{";
  output << "\"frame_count\":" << render_summary_.frame_count << ",";
  output << "\"pixel_width\":" << render_summary_.pixel_width << ",";
  output << "\"pixel_height\":" << render_summary_.pixel_height << ",";
  output << "\"frame_rate\":" << render_summary_.frame_rate << ",";
  output << "\"format\":\"" << escape_json(render_summary_.format) << "\",";
  output << "\"codec_hint\":\"" << escape_json(render_summary_.codec_hint) << "\",";
  output << "\"duration_seconds\":" << render_summary_.duration_seconds << ",";
  if (render_summary_.output_file.has_value()) {
    output << "\"output_file\":\""
           << escape_json(render_summary_.output_file->generic_string()) << "\"";
  } else {
    output << "\"output_file\":null";
  }
  output << "}";

  output << "}";
  return output.good();
}

std::string SceneFileWriter::make_partial_movie_file_name(
    const std::size_t index) const {
  std::ostringstream stream;
  stream << scene_name_ << "_partial_" << std::setfill('0') << std::setw(4)
         << index << ".mp4";
  return stream.str();
}

}  // namespace manim_cpp::scene
