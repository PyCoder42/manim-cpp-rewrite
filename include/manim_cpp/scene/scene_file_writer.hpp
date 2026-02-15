#pragma once

#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "manim_cpp/config/config.hpp"
#include "manim_cpp/scene/section.hpp"

namespace manim_cpp::scene {

struct Subcaption {
  std::string content;
  double start_seconds = 0.0;
  double end_seconds = 0.0;
};

struct AudioSegment {
  std::string path;
  double start_seconds = 0.0;
  double gain_db = 0.0;
};

struct SceneOutputPaths {
  std::filesystem::path images_dir;
  std::filesystem::path video_dir;
  std::filesystem::path partial_movie_dir;
};

struct RenderSummary {
  std::size_t frame_count = 0;
  std::size_t pixel_width = 0;
  std::size_t pixel_height = 0;
  double frame_rate = 0.0;
  std::string format = "mp4";
  std::string codec_hint = "unknown";
  double duration_seconds = 0.0;
  std::optional<std::filesystem::path> output_file;
};

class SceneFileWriter {
 public:
  explicit SceneFileWriter(std::string scene_name);

  void begin_section(const std::string& name, bool skip_animations);

  void begin_animation(bool write_frames);
  void end_animation(bool write_frames);

  void add_partial_movie_file(const std::string& path);
  void set_section_timeline(double start_seconds, double end_seconds);
  void add_subcaption(const std::string& content,
                      double start_seconds,
                      double end_seconds);
  void add_audio_segment(const std::string& path,
                         double start_seconds,
                         double gain_db);
  void set_render_summary(std::size_t frame_count,
                          std::size_t pixel_width,
                          std::size_t pixel_height,
                          double frame_rate,
                          const std::string& format,
                          std::optional<std::filesystem::path> output_file);
  std::optional<SceneOutputPaths> resolve_output_paths(
      const manim_cpp::config::ManimConfig& config,
      const std::string& module_name,
      const std::string& quality) const;
  bool write_subcaptions_srt(const std::filesystem::path& output_path) const;
  bool write_media_manifest(const std::filesystem::path& output_path) const;

  const std::string& scene_name() const { return scene_name_; }
  const std::vector<Section>& sections() const { return sections_; }
  const std::vector<Subcaption>& subcaptions() const { return subcaptions_; }
  const std::vector<AudioSegment>& audio_segments() const { return audio_segments_; }
  const RenderSummary& render_summary() const { return render_summary_; }

 private:
  std::string scene_name_;
  std::vector<Section> sections_;
  std::vector<Subcaption> subcaptions_;
  std::vector<AudioSegment> audio_segments_;
  RenderSummary render_summary_;
  bool animation_active_ = false;
  bool active_animation_writes_frames_ = false;
  std::size_t rendered_animation_count_ = 0;

  std::string make_partial_movie_file_name(std::size_t index) const;
};

}  // namespace manim_cpp::scene
