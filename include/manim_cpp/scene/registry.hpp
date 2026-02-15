#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "manim_cpp/scene/scene.hpp"

namespace manim_cpp::scene {

using SceneFactory = std::function<std::unique_ptr<Scene>()>;

class SceneRegistry {
 public:
  static SceneRegistry& instance();

  bool register_scene(const std::string& scene_name, SceneFactory factory);
  std::unique_ptr<Scene> create(const std::string& scene_name) const;
  std::vector<std::string> list_scene_names() const;

 private:
  SceneRegistry() = default;

  mutable std::mutex mutex_;
  std::unordered_map<std::string, SceneFactory> factories_;
};

template <typename SceneType>
class SceneRegistrar {
 public:
  explicit SceneRegistrar(const char* scene_name) {
    SceneRegistry::instance().register_scene(
        scene_name, []() { return std::make_unique<SceneType>(); });
  }
};

}  // namespace manim_cpp::scene

#define MANIM_REGISTER_SCENE(SceneType)                                \
  namespace {                                                          \
  const manim_cpp::scene::SceneRegistrar<SceneType>                   \
      kManimSceneRegistrar##SceneType{#SceneType};                    \
  }  // namespace
