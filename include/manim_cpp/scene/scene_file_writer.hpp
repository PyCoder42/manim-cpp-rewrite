#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "manim_cpp/scene/section.hpp"

namespace manim_cpp::scene {

class SceneFileWriter {
 public:
  explicit SceneFileWriter(std::string scene_name);

  void begin_animation(bool write_frames);
  void end_animation(bool write_frames);

  void add_partial_movie_file(const std::string& path);
  void add_subcaption(const std::string& content,
                      double start_seconds,
                      double end_seconds);

  const std::string& scene_name() const { return scene_name_; }
  const std::vector<Section>& sections() const { return sections_; }

 private:
  std::string scene_name_;
  std::vector<Section> sections_;
};

}  // namespace manim_cpp::scene
