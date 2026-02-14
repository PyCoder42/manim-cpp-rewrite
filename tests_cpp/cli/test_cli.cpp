#include <array>
#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "manim_cpp/cli/cli.hpp"

TEST(Cli, HelpAndSubcommandDispatchSmoke) {
  const std::array<const char*, 2> help_args = {"manim-cpp", "--help"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(help_args.size()),
                                    help_args.data()),
            0);

  const std::array<const char*, 3> render_help_args = {
      "manim-cpp", "render", "--help"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(render_help_args.size()),
                                    render_help_args.data()),
            0);
}

TEST(Cli, ReturnsNonZeroForInvalidSubcommandUsage) {
  const std::array<const char*, 3> cfg_invalid_args = {"manim-cpp", "cfg", "bogus"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(cfg_invalid_args.size()),
                                    cfg_invalid_args.data()),
            2);

  const std::array<const char*, 2> render_missing_args = {"manim-cpp", "render"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(render_missing_args.size()),
                                    render_missing_args.data()),
            2);
}

TEST(Cli, AcceptsKnownScaffoldedSubcommands) {
  const std::array<const char*, 3> plugins_list_args = {"manim-cpp", "plugins", "list"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(plugins_list_args.size()),
                                    plugins_list_args.data()),
            2);
}

TEST(Cli, CfgShowAndWriteOperateOnFiles) {
  const auto temp_root = std::filesystem::temp_directory_path() / "manim_cpp_cli_cfg";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const auto source_cfg_path = temp_root / "source.cfg";
  std::ofstream source_cfg(source_cfg_path);
  source_cfg << "[CLI]\nrenderer = cairo\n";
  source_cfg.close();

  const auto source_cfg_string = source_cfg_path.string();
  const std::array<const char*, 4> show_args = {
      "manim-cpp", "cfg", "show", source_cfg_string.c_str()};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(show_args.size()),
                                    show_args.data()),
            0);

  const auto generated_cfg_path = temp_root / "generated.cfg";
  const auto generated_cfg_string = generated_cfg_path.string();
  const std::array<const char*, 4> write_args = {
      "manim-cpp", "cfg", "write", generated_cfg_string.c_str()};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(write_args.size()),
                                    write_args.data()),
            0);
  EXPECT_TRUE(std::filesystem::exists(generated_cfg_path));

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, PluginsListReadsDirectory) {
  const auto temp_root = std::filesystem::temp_directory_path() / "manim_cpp_cli_plugins";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

#ifdef _WIN32
  const auto plugin_path = temp_root / "sample.dll";
#elif __APPLE__
  const auto plugin_path = temp_root / "sample.dylib";
#else
  const auto plugin_path = temp_root / "sample.so";
#endif
  std::ofstream plugin_file(plugin_path);
  plugin_file << "x";
  plugin_file.close();

  const auto path_string = temp_root.string();
  const std::array<const char*, 4> args = {
      "manim-cpp", "plugins", "list", path_string.c_str()};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 0);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, CheckhealthJsonModeIsAccepted) {
  const std::array<const char*, 3> args = {"manim-cpp", "checkhealth", "--json"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 0);
}

TEST(Cli, InitSceneGeneratesTemplateFile) {
  const auto temp_root = std::filesystem::temp_directory_path() / "manim_cpp_cli_init";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const auto scene_path = temp_root / "my_scene.cpp";
  const auto scene_path_string = scene_path.string();
  const std::array<const char*, 4> args = {
      "manim-cpp", "init", "scene", scene_path_string.c_str()};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 0);
  EXPECT_TRUE(std::filesystem::exists(scene_path));

  std::ifstream scene_file(scene_path);
  const std::string contents((std::istreambuf_iterator<char>(scene_file)),
                             std::istreambuf_iterator<char>());
  EXPECT_NE(contents.find("MANIM_REGISTER_SCENE"), std::string::npos);

  std::filesystem::remove_all(temp_root);
}
