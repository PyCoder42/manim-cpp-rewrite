#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <random>
#include <string>
#include <vector>

namespace manim_cpp::animation {
class Animation;
}  // namespace manim_cpp::animation

namespace manim_cpp::scene {

using SceneUpdater = std::function<void(double)>;

class Scene {
 public:
  virtual ~Scene() = default;

  virtual void setup() {}
  virtual void construct() = 0;
  virtual void tear_down() {}

  void run();
  void play(animation::Animation& animation, std::size_t steps = 60);
  void tick(double delta_seconds);

  void add_updater(SceneUpdater updater);
  void clear_updaters();

  [[nodiscard]] double time_seconds() const;
  void set_random_seed(std::uint64_t seed);
  [[nodiscard]] std::uint64_t random_seed() const;
  double random_unit();

  virtual std::string scene_name() const { return "Scene"; }

 private:
  std::vector<SceneUpdater> updaters_;
  double elapsed_seconds_ = 0.0;
  std::uint64_t random_seed_ = 0;
  std::mt19937_64 rng_{0};
};

}  // namespace manim_cpp::scene
