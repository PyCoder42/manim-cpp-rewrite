#pragma once

#include <string>
#include <utility>
#include <vector>

namespace manim_cpp::scene {

class Section {
 public:
  Section(std::string name, bool skip_animations)
      : name_(std::move(name)), skip_animations_(skip_animations) {}

  const std::string& name() const { return name_; }
  bool skip_animations() const { return skip_animations_; }

  void add_partial_movie_file(std::string path) {
    partial_movie_files_.push_back(std::move(path));
  }

  const std::vector<std::string>& partial_movie_files() const {
    return partial_movie_files_;
  }

 private:
  std::string name_;
  bool skip_animations_ = false;
  std::vector<std::string> partial_movie_files_;
};

}  // namespace manim_cpp::scene
