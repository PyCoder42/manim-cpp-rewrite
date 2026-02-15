#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/scene/registry.hpp"
#include "manim_cpp/scene/scene.hpp"

namespace {

TEST(ExampleScenes, RegistryCanCreateConvertedExampleScenes) {
  const std::vector<std::string> expected_scene_names{
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

  for (const auto& scene_name : expected_scene_names) {
    auto scene = manim_cpp::scene::SceneRegistry::instance().create(scene_name);
    ASSERT_NE(scene, nullptr) << "Missing scene registration: " << scene_name;
    EXPECT_EQ(scene->scene_name(), scene_name);
  }
}

TEST(ExampleScenes, ConvertedBasicScenesRunWithoutThrowing) {
  const std::vector<std::string> runnable_scene_names{
      "OpeningManim",
      "SquareToCircle",
      "WarpSquare",
      "WriteStuff",
      "UpdatersExample",
      "SpiralInExample",
      "LineJoints",
  };

  for (const auto& scene_name : runnable_scene_names) {
    auto scene = manim_cpp::scene::SceneRegistry::instance().create(scene_name);
    ASSERT_NE(scene, nullptr) << "Missing scene registration: " << scene_name;
    EXPECT_NO_THROW(scene->run()) << "Scene failed to run: " << scene_name;
  }
}

}  // namespace
