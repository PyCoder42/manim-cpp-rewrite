#include <filesystem>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include "manim_cpp/scene/scene_file_writer.hpp"

namespace {

std::string read_file(const std::filesystem::path& path) {
  std::ifstream input(path);
  return std::string((std::istreambuf_iterator<char>(input)),
                     std::istreambuf_iterator<char>());
}

}  // namespace

TEST(SceneFileWriter, WritesSubcaptionsAsSrt) {
  manim_cpp::scene::SceneFileWriter writer("TestScene");
  writer.add_subcaption("Hello", 0.0, 1.5);
  writer.add_subcaption("World", 1.5, 2.0);

  const auto output_path =
      std::filesystem::temp_directory_path() / "manim_cpp_scene_file_writer_test.srt";
  std::filesystem::remove(output_path);

  ASSERT_TRUE(writer.write_subcaptions_srt(output_path));
  const auto content = read_file(output_path);

  const std::string expected =
      "1\n"
      "00:00:00,000 --> 00:00:01,500\n"
      "Hello\n"
      "\n"
      "2\n"
      "00:00:01,500 --> 00:00:02,000\n"
      "World\n"
      "\n";
  EXPECT_EQ(content, expected);
  std::filesystem::remove(output_path);
}

TEST(SceneFileWriter, RejectsInvalidSubcaptionRanges) {
  manim_cpp::scene::SceneFileWriter writer("TestScene");
  writer.add_subcaption("Invalid", 3.0, 2.0);
  writer.add_subcaption("AlsoInvalid", 2.0, 2.0);
  EXPECT_TRUE(writer.subcaptions().empty());
}
