#include "manim_cpp/scene/scene.hpp"

#include <utility>

#include "manim_cpp/animation/animation.hpp"

namespace manim_cpp::scene {

void Scene::run() {
  setup();
  construct();
  tear_down();
}

void Scene::play(animation::Animation& animation, const std::size_t steps) {
  animation.begin();
  const double run_time_seconds = animation.run_time_seconds();
  if (steps == 0) {
    animation.interpolate_with_rate(1.0);
    tick(run_time_seconds);
    animation.finish();
    return;
  }

  const double delta_seconds = run_time_seconds / static_cast<double>(steps);
  for (std::size_t frame = 0; frame <= steps; ++frame) {
    const double alpha = static_cast<double>(frame) / static_cast<double>(steps);
    animation.interpolate_with_rate(alpha);
    tick(delta_seconds);
  }
  animation.finish();
}

void Scene::tick(const double delta_seconds) {
  elapsed_seconds_ += delta_seconds;
  for (const auto& updater : updaters_) {
    updater(delta_seconds);
  }
}

void Scene::add_updater(SceneUpdater updater) {
  updaters_.push_back(std::move(updater));
}

void Scene::clear_updaters() {
  updaters_.clear();
}

double Scene::time_seconds() const {
  return elapsed_seconds_;
}

void Scene::set_random_seed(const std::uint64_t seed) {
  random_seed_ = seed;
  rng_.seed(seed);
}

std::uint64_t Scene::random_seed() const {
  return random_seed_;
}

double Scene::random_unit() {
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  return distribution(rng_);
}

}  // namespace manim_cpp::scene
