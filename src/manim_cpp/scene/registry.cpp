#include "manim_cpp/scene/registry.hpp"

#include <algorithm>

namespace manim_cpp::scene {

SceneRegistry& SceneRegistry::instance() {
  static SceneRegistry registry;
  return registry;
}

bool SceneRegistry::register_scene(const std::string& scene_name,
                                   SceneFactory factory) {
  std::scoped_lock lock(mutex_);
  auto [it, inserted] = factories_.emplace(scene_name, std::move(factory));
  if (!inserted) {
    it->second = std::move(factory);
  }
  return inserted;
}

std::unique_ptr<Scene> SceneRegistry::create(
    const std::string& scene_name) const {
  std::scoped_lock lock(mutex_);
  const auto it = factories_.find(scene_name);
  if (it == factories_.end()) {
    return nullptr;
  }
  return it->second();
}

std::vector<std::string> SceneRegistry::list_scene_names() const {
  std::scoped_lock lock(mutex_);
  std::vector<std::string> names;
  names.reserve(factories_.size());
  for (const auto& [name, _] : factories_) {
    names.push_back(name);
  }
  std::sort(names.begin(), names.end());
  return names;
}

}  // namespace manim_cpp::scene
