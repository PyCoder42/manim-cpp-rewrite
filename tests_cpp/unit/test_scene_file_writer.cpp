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

TEST(SceneFileWriter, TracksExplicitSectionsAndPartialFiles) {
  manim_cpp::scene::SceneFileWriter writer("TestScene");
  writer.begin_section("Intro", false);
  writer.add_partial_movie_file("intro_0001.mp4");
  writer.begin_section("Outro", true);
  writer.add_partial_movie_file("outro_0001.mp4");

  ASSERT_EQ(writer.sections().size(), static_cast<size_t>(3));
  EXPECT_EQ(writer.sections()[1].name(), std::string("Intro"));
  EXPECT_FALSE(writer.sections()[1].skip_animations());
  ASSERT_EQ(writer.sections()[1].partial_movie_files().size(), static_cast<size_t>(1));
  EXPECT_EQ(writer.sections()[1].partial_movie_files()[0], std::string("intro_0001.mp4"));

  EXPECT_EQ(writer.sections()[2].name(), std::string("Outro"));
  EXPECT_TRUE(writer.sections()[2].skip_animations());
  ASSERT_EQ(writer.sections()[2].partial_movie_files().size(), static_cast<size_t>(1));
  EXPECT_EQ(writer.sections()[2].partial_movie_files()[0], std::string("outro_0001.mp4"));
}

TEST(SceneFileWriter, TracksAudioSegmentsForLayeredMixing) {
  manim_cpp::scene::SceneFileWriter writer("TestScene");
  writer.add_audio_segment("narration.wav", 0.0, 0.0);
  writer.add_audio_segment("music.wav", 1.25, -6.0);

  ASSERT_EQ(writer.audio_segments().size(), static_cast<size_t>(2));
  EXPECT_EQ(writer.audio_segments()[0].path, std::string("narration.wav"));
  EXPECT_DOUBLE_EQ(writer.audio_segments()[0].start_seconds, 0.0);
  EXPECT_DOUBLE_EQ(writer.audio_segments()[0].gain_db, 0.0);

  EXPECT_EQ(writer.audio_segments()[1].path, std::string("music.wav"));
  EXPECT_DOUBLE_EQ(writer.audio_segments()[1].start_seconds, 1.25);
  EXPECT_DOUBLE_EQ(writer.audio_segments()[1].gain_db, -6.0);
}

TEST(SceneFileWriter, RejectsInvalidAudioSegments) {
  manim_cpp::scene::SceneFileWriter writer("TestScene");
  writer.add_audio_segment("", 0.0, 0.0);
  writer.add_audio_segment("track.wav", -0.1, 0.0);

  EXPECT_TRUE(writer.audio_segments().empty());
}

TEST(SceneFileWriter, AutoTracksDeterministicPartialMovieFilesFromAnimations) {
  manim_cpp::scene::SceneFileWriter writer("DemoScene");
  writer.begin_animation(true);
  writer.end_animation(true);
  writer.begin_animation(true);
  writer.end_animation(true);

  ASSERT_EQ(writer.sections().size(), static_cast<size_t>(1));
  const auto& files = writer.sections()[0].partial_movie_files();
  ASSERT_EQ(files.size(), static_cast<size_t>(2));
  EXPECT_EQ(files[0], std::string("DemoScene_partial_0001.mp4"));
  EXPECT_EQ(files[1], std::string("DemoScene_partial_0002.mp4"));
}

TEST(SceneFileWriter, SkipsAutoPartialMovieFilesWhenFramesAreDisabledOrSkipped) {
  manim_cpp::scene::SceneFileWriter writer("DemoScene");
  writer.begin_animation(false);
  writer.end_animation(false);

  writer.begin_section("SkipAnimations", true);
  writer.begin_animation(true);
  writer.end_animation(true);

  ASSERT_EQ(writer.sections().size(), static_cast<size_t>(2));
  EXPECT_TRUE(writer.sections()[0].partial_movie_files().empty());
  EXPECT_TRUE(writer.sections()[1].partial_movie_files().empty());
}
