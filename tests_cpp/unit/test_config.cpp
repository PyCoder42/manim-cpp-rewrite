#include <filesystem>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include "manim_cpp/config/config.hpp"

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
