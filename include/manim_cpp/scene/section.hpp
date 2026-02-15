#pragma once

#include <optional>
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

  void set_timeline(double start_seconds, double end_seconds) {
    if (end_seconds <= start_seconds || start_seconds < 0.0) {
      return;
    }
    start_seconds_ = start_seconds;
    end_seconds_ = end_seconds;
  }

  const std::optional<double>& start_seconds() const { return start_seconds_; }
  const std::optional<double>& end_seconds() const { return end_seconds_; }

 private:
  std::string name_;
  bool skip_animations_ = false;
  std::vector<std::string> partial_movie_files_;
  std::optional<double> start_seconds_;
  std::optional<double> end_seconds_;
};

}  // namespace manim_cpp::scene
