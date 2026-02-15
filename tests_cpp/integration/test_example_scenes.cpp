#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/scene/registry.hpp"
#include "manim_cpp/scene/scene.hpp"

namespace {

std::vector<std::string> expected_scene_names() {
  return {
      "OpeningManim",
      "SquareToCircle",
      "WarpSquare",
      "WriteStuff",
      "UpdatersExample",
      "SpiralInExample",
      "LineJoints",
      "TexFontTemplateManual",
      "TexFontTemplateLibrary",
      "TexTemplateFromCLI",
      "InCodeTexTemplate",
      "TextTest",
      "GuiTest",
      "GuiTest2",
      "ThreeDMobjectTest",
      "NamedFullScreenQuad",
      "InlineFullScreenQuad",
      "SimpleInlineFullScreenQuad",
      "InlineShaderExample",
      "NamedShaderExample",
      "InteractiveDevelopment",
      "SurfaceExample",
  };
}

TEST(ExampleScenes, RegistryCanCreateConvertedExampleScenes) {
  const auto scenes = expected_scene_names();

  for (const auto& scene_name : scenes) {
    auto scene = manim_cpp::scene::SceneRegistry::instance().create(scene_name);
    ASSERT_NE(scene, nullptr) << "Missing scene registration: " << scene_name;
    EXPECT_EQ(scene->scene_name(), scene_name);
  }
}

TEST(ExampleScenes, ConvertedExampleScenesRunWithoutThrowing) {
  const auto scenes = expected_scene_names();

  for (const auto& scene_name : scenes) {
    auto scene = manim_cpp::scene::SceneRegistry::instance().create(scene_name);
    ASSERT_NE(scene, nullptr) << "Missing scene registration: " << scene_name;
    EXPECT_NO_THROW(scene->run()) << "Scene failed to run: " << scene_name;
  }
}

}  // namespace
