#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "manim_cpp/scene/registry.hpp"
#include "manim_cpp/scene/scene.hpp"

namespace {

class RegistrySmokeScene : public manim_cpp::scene::Scene {
 public:
  std::string scene_name() const override { return "RegistrySmokeScene"; }
  void construct() override {}
};

MANIM_REGISTER_SCENE(RegistrySmokeScene);

}  // namespace

TEST(SceneRegistry, CreatesRegisteredSceneByName) {
  auto scene = manim_cpp::scene::SceneRegistry::instance().create(
      "RegistrySmokeScene");
  ASSERT_NE(scene, nullptr);
  EXPECT_EQ(scene->scene_name(), std::string("RegistrySmokeScene"));
}
