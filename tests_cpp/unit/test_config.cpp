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
