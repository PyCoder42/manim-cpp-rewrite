#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

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

class SceneFileWriter {
 public:
  explicit SceneFileWriter(std::string scene_name);

  void begin_section(const std::string& name, bool skip_animations);

  void begin_animation(bool write_frames);
  void end_animation(bool write_frames);

  void add_partial_movie_file(const std::string& path);
  void add_subcaption(const std::string& content,
                      double start_seconds,
                      double end_seconds);
  void add_audio_segment(const std::string& path,
                         double start_seconds,
                         double gain_db);
  bool write_subcaptions_srt(const std::filesystem::path& output_path) const;

  const std::string& scene_name() const { return scene_name_; }
  const std::vector<Section>& sections() const { return sections_; }
  const std::vector<Subcaption>& subcaptions() const { return subcaptions_; }
  const std::vector<AudioSegment>& audio_segments() const { return audio_segments_; }

 private:
  std::string scene_name_;
  std::vector<Section> sections_;
  std::vector<Subcaption> subcaptions_;
  std::vector<AudioSegment> audio_segments_;
  bool animation_active_ = false;
  bool active_animation_writes_frames_ = false;
  std::size_t rendered_animation_count_ = 0;

  std::string make_partial_movie_file_name(std::size_t index) const;
};

}  // namespace manim_cpp::scene
