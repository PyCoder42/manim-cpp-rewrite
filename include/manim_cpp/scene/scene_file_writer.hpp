#pragma once

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
  bool write_subcaptions_srt(const std::filesystem::path& output_path) const;

  const std::string& scene_name() const { return scene_name_; }
  const std::vector<Section>& sections() const { return sections_; }
  const std::vector<Subcaption>& subcaptions() const { return subcaptions_; }

 private:
  std::string scene_name_;
  std::vector<Section> sections_;
  std::vector<Subcaption> subcaptions_;
};

}  // namespace manim_cpp::scene
