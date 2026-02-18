#include <array>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/cli/cli.hpp"
#include "manim_cpp/scene/registry.hpp"
#include "manim_cpp/scene/scene.hpp"

namespace {

class ScopedCurrentPath {
 public:
  explicit ScopedCurrentPath(const std::filesystem::path& next_path)
      : previous_(std::filesystem::current_path()) {
    std::filesystem::current_path(next_path);
  }

  ~ScopedCurrentPath() { std::filesystem::current_path(previous_); }

 private:
  std::filesystem::path previous_;
};

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

TEST(ExampleScenes, ConvertedExampleScenesRenderViaCliWithArtifacts) {
  const auto scenes = expected_scene_names();
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_example_scene_cli_render";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  std::ofstream cfg(temp_root / "manim.cfg");
  cfg << "[CLI]\n";
  cfg << "media_dir = ./media\n";
  cfg << "video_dir = {media_dir}/videos/{module_name}/{quality}\n";
  cfg << "images_dir = {media_dir}/images/{module_name}\n";
  cfg << "partial_movie_dir = {video_dir}/partial_movie_files/{scene_name}\n";
  cfg << "pixel_width = 854\n";
  cfg << "pixel_height = 480\n";
  cfg << "frame_rate = 24\n";
  cfg.close();

  std::ofstream input_scene(temp_root / "example_scenes.cpp");
  input_scene << "// placeholder\n";
  input_scene.close();

  {
    ScopedCurrentPath scoped_path(temp_root);
    for (const auto& scene_name : scenes) {
      const std::array<std::string, 7> args_storage = {
          "manim-cpp",
          "render",
          "example_scenes.cpp",
          "--scene",
          scene_name,
          "--renderer",
          "cairo",
      };
      const std::array<const char*, 7> args = {
          args_storage[0].c_str(), args_storage[1].c_str(), args_storage[2].c_str(),
          args_storage[3].c_str(), args_storage[4].c_str(), args_storage[5].c_str(),
          args_storage[6].c_str(),
      };
      EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 0)
          << "Failed scene render via CLI: " << scene_name;
    }
  }

  const auto output_root =
      temp_root / "media" / "videos" / "example_scenes" / "480p24";
  for (const auto& scene_name : scenes) {
    EXPECT_TRUE(std::filesystem::exists(output_root / (scene_name + ".mp4")))
        << "Missing media artifact for " << scene_name;
    EXPECT_TRUE(std::filesystem::exists(output_root / (scene_name + ".json")))
        << "Missing manifest artifact for " << scene_name;
    EXPECT_TRUE(std::filesystem::exists(output_root / (scene_name + ".srt")))
        << "Missing subtitle artifact for " << scene_name;
  }

  std::filesystem::remove_all(temp_root);
}

}  // namespace
