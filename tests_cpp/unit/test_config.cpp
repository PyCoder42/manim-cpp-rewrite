#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

#include <gtest/gtest.h>

#include "manim_cpp/config/config.hpp"

namespace {

std::filesystem::path find_repo_root() {
  auto probe = std::filesystem::current_path();
  for (int depth = 0; depth < 12; ++depth) {
    if (std::filesystem::exists(probe / "config" / "manim.cfg.default")) {
      return probe;
    }
    if (!probe.has_parent_path()) {
      break;
    }
    probe = probe.parent_path();
  }
  return {};
}

}  // namespace

TEST(ConfigLoader, ReadsManimCfgSectionsAndKeys) {
  const auto temp_dir = std::filesystem::temp_directory_path() / "manim_cpp_tests";
  std::filesystem::create_directories(temp_dir);
  const auto cfg_path = temp_dir / "manim.cfg";

  std::ofstream cfg(cfg_path);
  cfg << "[CLI]\n";
  cfg << "renderer = cairo\n";
  cfg << "frame_rate = 60\n";
  cfg << "[logger]\n";
  cfg << "log_level = INFO\n";
  cfg.close();

  manim_cpp::config::ManimConfig config;
  ASSERT_TRUE(config.load_from_file(cfg_path));
  EXPECT_EQ(config.get("CLI", "renderer", ""), std::string("cairo"));
  EXPECT_EQ(config.get("CLI", "frame_rate", ""), std::string("60"));
  EXPECT_EQ(config.get("logger", "log_level", ""), std::string("INFO"));
}

TEST(ConfigLoader, AppliesLaterFilesAsHigherPrecedence) {
  const auto temp_dir =
      std::filesystem::temp_directory_path() / "manim_cpp_tests_precedence";
  std::filesystem::create_directories(temp_dir);

  const auto base_cfg_path = temp_dir / "base.cfg";
  std::ofstream base_cfg(base_cfg_path);
  base_cfg << "[CLI]\n";
  base_cfg << "renderer = cairo\n";
  base_cfg << "frame_rate = 30\n";
  base_cfg.close();

  const auto user_cfg_path = temp_dir / "user.cfg";
  std::ofstream user_cfg(user_cfg_path);
  user_cfg << "[CLI]\n";
  user_cfg << "frame_rate = 60\n";
  user_cfg << "background_color = BLACK\n";
  user_cfg.close();

  manim_cpp::config::ManimConfig config;
  ASSERT_TRUE(config.load_with_precedence({base_cfg_path, user_cfg_path}));
  EXPECT_EQ(config.get("CLI", "renderer", ""), std::string("cairo"));
  EXPECT_EQ(config.get("CLI", "frame_rate", ""), std::string("60"));
  EXPECT_EQ(config.get("CLI", "background_color", ""), std::string("BLACK"));
}

TEST(ConfigLoader, LoadFromFileResetsPriorState) {
  const auto temp_dir =
      std::filesystem::temp_directory_path() / "manim_cpp_tests_reset";
  std::filesystem::create_directories(temp_dir);

  const auto first_cfg_path = temp_dir / "first.cfg";
  std::ofstream first_cfg(first_cfg_path);
  first_cfg << "[CLI]\n";
  first_cfg << "renderer = cairo\n";
  first_cfg.close();

  const auto second_cfg_path = temp_dir / "second.cfg";
  std::ofstream second_cfg(second_cfg_path);
  second_cfg << "[CLI]\n";
  second_cfg << "frame_rate = 24\n";
  second_cfg.close();

  manim_cpp::config::ManimConfig config;
  ASSERT_TRUE(config.load_from_file(first_cfg_path));
  ASSERT_TRUE(config.load_from_file(second_cfg_path));
  EXPECT_EQ(config.get("CLI", "frame_rate", ""), std::string("24"));
  EXPECT_FALSE(config.has("CLI", "renderer"));
}

TEST(ConfigLoader, ResolvesNestedCliDirectoryTemplates) {
  const auto repo_root = find_repo_root();
  ASSERT_FALSE(repo_root.empty());

  manim_cpp::config::ManimConfig config;
  ASSERT_TRUE(config.load_from_file(repo_root / "config" / "manim.cfg.default"));

  const auto substitutions = std::unordered_map<std::string, std::string>{
      {"module_name", "demo_module"},
      {"quality", "1080p60"},
      {"scene_name", "DemoScene"},
  };

  const auto video_dir = config.resolve_path("CLI", "video_dir", substitutions);
  ASSERT_TRUE(video_dir.has_value());
  EXPECT_EQ(video_dir->generic_string(), std::string("./media/videos/demo_module/1080p60"));

  const auto partial_movie_dir =
      config.resolve_path("CLI", "partial_movie_dir", substitutions);
  ASSERT_TRUE(partial_movie_dir.has_value());
  EXPECT_EQ(partial_movie_dir->generic_string(),
            std::string("./media/videos/demo_module/1080p60/partial_movie_files/DemoScene"));
}

TEST(ConfigLoader, ResolvesCustomFolderTemplates) {
  const auto temp_dir =
      std::filesystem::temp_directory_path() / "manim_cpp_tests_custom_folders";
  std::filesystem::create_directories(temp_dir);

  const auto cfg_path = temp_dir / "custom_folders.cfg";
  std::ofstream cfg(cfg_path);
  cfg << "[custom_folders]\n";
  cfg << "media_dir = videos\n";
  cfg << "video_dir = {media_dir}\n";
  cfg << "partial_movie_dir = {video_dir}/partial_movie_files/{scene_name}\n";
  cfg.close();

  manim_cpp::config::ManimConfig config;
  ASSERT_TRUE(config.load_from_file(cfg_path));

  const auto resolved = config.resolve_path(
      "custom_folders", "partial_movie_dir", {{"scene_name", "IntroScene"}});
  ASSERT_TRUE(resolved.has_value());
  EXPECT_EQ(resolved->generic_string(),
            std::string("videos/partial_movie_files/IntroScene"));
}

TEST(ConfigLoader, ReturnsNulloptWhenTemplateVariableIsMissing) {
  const auto repo_root = find_repo_root();
  ASSERT_FALSE(repo_root.empty());

  manim_cpp::config::ManimConfig config;
  ASSERT_TRUE(config.load_from_file(repo_root / "config" / "manim.cfg.default"));

  const auto unresolved =
      config.resolve_path("CLI", "video_dir", {{"module_name", "demo_module"}});
  EXPECT_FALSE(unresolved.has_value());
}
