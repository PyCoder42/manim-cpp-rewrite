#include "manim_cpp/scene/scene_file_writer.hpp"

namespace manim_cpp::scene {

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

void SceneFileWriter::add_subcaption(const std::string& /*content*/,
                                     double /*start_seconds*/,
                                     double /*end_seconds*/) {}

}  // namespace manim_cpp::scene
