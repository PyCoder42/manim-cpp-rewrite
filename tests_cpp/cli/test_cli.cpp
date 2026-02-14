#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

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

TEST(Cli, RenderValidatesRendererOption) {
  const std::array<const char*, 5> valid_args = {
      "manim-cpp", "render", "example_scene.cpp", "--renderer", "opengl"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(valid_args.size()), valid_args.data()),
            0);

  const std::array<const char*, 5> invalid_args = {
      "manim-cpp", "render", "example_scene.cpp", "--renderer", "metal"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(invalid_args.size()), invalid_args.data()),
            2);
}

TEST(Cli, RenderAcceptsWatchAndInteractiveFlags) {
  const std::array<const char*, 7> args = {
      "manim-cpp", "render", "example_scene.cpp", "--renderer", "opengl", "--watch",
      "--interactive"};

  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find("watch=true"), std::string::npos);
  EXPECT_NE(out_capture.str().find("interactive=true"), std::string::npos);
}

TEST(Cli, RenderValidatesFormatOption) {
  const std::array<const char*, 7> valid_args = {"manim-cpp",
                                                  "render",
                                                  "example_scene.cpp",
                                                  "--renderer",
                                                  "cairo",
                                                  "--format",
                                                  "webm"};
  std::ostringstream valid_out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(valid_out_capture.rdbuf());
  const int valid_exit_code =
      manim_cpp::cli::run_cli(static_cast<int>(valid_args.size()), valid_args.data());
  std::cout.rdbuf(old_cout);
  EXPECT_EQ(valid_exit_code, 0);
  EXPECT_NE(valid_out_capture.str().find("format=webm"), std::string::npos);

  const std::array<const char*, 7> invalid_args = {"manim-cpp",
                                                    "render",
                                                    "example_scene.cpp",
                                                    "--renderer",
                                                    "cairo",
                                                    "--format",
                                                    "avi"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(invalid_args.size()),
                                    invalid_args.data()),
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

TEST(Cli, PluginsLoadAcceptsEmptyDirectory) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_plugins_load_empty";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const auto path_string = temp_root.string();
  const std::array<const char*, 4> args = {
      "manim-cpp", "plugins", "load", path_string.c_str()};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 0);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, PluginsLoadReportsSceneRegistrationsFromPluginInit) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_plugins_load_real";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const std::filesystem::path fixture_path = MANIM_CPP_TEST_PLUGIN_FIXTURE_PATH;
  ASSERT_TRUE(std::filesystem::exists(fixture_path));

  const auto copied_plugin = temp_root / fixture_path.filename();
  ASSERT_TRUE(std::filesystem::copy_file(
      fixture_path, copied_plugin, std::filesystem::copy_options::overwrite_existing));

  const auto path_string = temp_root.string();
  const std::array<const char*, 4> args = {
      "manim-cpp", "plugins", "load", path_string.c_str()};

  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find("Loaded 1 plugin(s)"), std::string::npos);
  EXPECT_NE(out_capture.str().find("FixtureScene"), std::string::npos);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, CheckhealthJsonModeIsAccepted) {
  const std::array<const char*, 3> args = {"manim-cpp", "checkhealth", "--json"};
  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find("\"renderers\":[\"cairo\",\"opengl\"]"),
            std::string::npos);
  EXPECT_NE(out_capture.str().find("\"formats\":[\"png\",\"gif\",\"mp4\",\"webm\",\"mov\"]"),
            std::string::npos);
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

TEST(Cli, InitProjectGeneratesProjectScaffold) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_init_project";
  std::filesystem::remove_all(temp_root);

  const auto project_root = temp_root / "demo_project";
  const auto project_root_string = project_root.string();
  const std::array<const char*, 4> args = {
      "manim-cpp", "init", "project", project_root_string.c_str()};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 0);

  const auto cfg_path = project_root / "manim.cfg";
  const auto scene_path = project_root / "scenes" / "main_scene.cpp";
  EXPECT_TRUE(std::filesystem::exists(cfg_path));
  EXPECT_TRUE(std::filesystem::exists(scene_path));

  std::ifstream scene_file(scene_path);
  const std::string contents((std::istreambuf_iterator<char>(scene_file)),
                             std::istreambuf_iterator<char>());
  EXPECT_NE(contents.find("MANIM_REGISTER_SCENE"), std::string::npos);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, PluginsListSupportsRecursiveDiscoveryFlag) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_plugins_recursive";
  std::filesystem::remove_all(temp_root);
  const auto nested_dir = temp_root / "nested";
  std::filesystem::create_directories(nested_dir);

#ifdef _WIN32
  const auto plugin_path = nested_dir / "sample.dll";
#elif __APPLE__
  const auto plugin_path = nested_dir / "sample.dylib";
#else
  const auto plugin_path = nested_dir / "sample.so";
#endif
  std::ofstream plugin_file(plugin_path);
  plugin_file << "x";
  plugin_file.close();

  const auto path_string = temp_root.string();
  const std::array<const char*, 5> args = {
      "manim-cpp", "plugins", "list", "--recursive", path_string.c_str()};

  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find("sample"), std::string::npos);

  std::filesystem::remove_all(temp_root);
}
