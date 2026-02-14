#include <gtest/gtest.h>

#include "manim_cpp/scene/media_format.hpp"

TEST(MediaFormat, ParsesKnownFormatsCaseInsensitively) {
  const auto png = manim_cpp::scene::parse_media_format("png");
  ASSERT_TRUE(png.has_value());
  EXPECT_EQ(png.value(), manim_cpp::scene::MediaFormat::kPng);

  const auto gif = manim_cpp::scene::parse_media_format("GIF");
  ASSERT_TRUE(gif.has_value());
  EXPECT_EQ(gif.value(), manim_cpp::scene::MediaFormat::kGif);

  const auto mp4 = manim_cpp::scene::parse_media_format("Mp4");
  ASSERT_TRUE(mp4.has_value());
  EXPECT_EQ(mp4.value(), manim_cpp::scene::MediaFormat::kMp4);

  const auto webm = manim_cpp::scene::parse_media_format("webm");
  ASSERT_TRUE(webm.has_value());
  EXPECT_EQ(webm.value(), manim_cpp::scene::MediaFormat::kWebm);

  const auto mov = manim_cpp::scene::parse_media_format("mov");
  ASSERT_TRUE(mov.has_value());
  EXPECT_EQ(mov.value(), manim_cpp::scene::MediaFormat::kMov);
}

TEST(MediaFormat, ConvertsFormatsToDeterministicLowercaseStrings) {
  EXPECT_EQ(manim_cpp::scene::to_string(manim_cpp::scene::MediaFormat::kPng),
            std::string("png"));
  EXPECT_EQ(manim_cpp::scene::to_string(manim_cpp::scene::MediaFormat::kGif),
            std::string("gif"));
  EXPECT_EQ(manim_cpp::scene::to_string(manim_cpp::scene::MediaFormat::kMp4),
            std::string("mp4"));
  EXPECT_EQ(manim_cpp::scene::to_string(manim_cpp::scene::MediaFormat::kWebm),
            std::string("webm"));
  EXPECT_EQ(manim_cpp::scene::to_string(manim_cpp::scene::MediaFormat::kMov),
            std::string("mov"));
}

TEST(MediaFormat, RejectsUnknownFormats) {
  EXPECT_FALSE(manim_cpp::scene::parse_media_format("avi").has_value());
  EXPECT_FALSE(manim_cpp::scene::parse_media_format("").has_value());
}
