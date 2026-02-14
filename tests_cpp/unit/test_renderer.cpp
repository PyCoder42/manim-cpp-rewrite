#include <gtest/gtest.h>

#include "manim_cpp/renderer/cairo_renderer.hpp"
#include "manim_cpp/renderer/renderer.hpp"

TEST(Renderer, ParsesKnownRendererNames) {
  const auto cairo = manim_cpp::renderer::parse_renderer_type("cairo");
  ASSERT_TRUE(cairo.has_value());
  EXPECT_EQ(cairo.value(), manim_cpp::renderer::RendererType::kCairo);

  const auto opengl = manim_cpp::renderer::parse_renderer_type("OpenGL");
  ASSERT_TRUE(opengl.has_value());
  EXPECT_EQ(opengl.value(), manim_cpp::renderer::RendererType::kOpenGL);

  EXPECT_FALSE(manim_cpp::renderer::parse_renderer_type("metal").has_value());
}

TEST(Renderer, CreatesConcreteRendererFromType) {
  auto cairo = manim_cpp::renderer::make_renderer(
      manim_cpp::renderer::RendererType::kCairo);
  ASSERT_NE(cairo, nullptr);
  EXPECT_EQ(cairo->name(), std::string("cairo"));
  EXPECT_EQ(cairo->type(), manim_cpp::renderer::RendererType::kCairo);

  auto opengl = manim_cpp::renderer::make_renderer(
      manim_cpp::renderer::RendererType::kOpenGL);
  ASSERT_NE(opengl, nullptr);
  EXPECT_EQ(opengl->name(), std::string("opengl"));
  EXPECT_EQ(opengl->type(), manim_cpp::renderer::RendererType::kOpenGL);
}

TEST(Renderer, ConvertsRendererTypeToString) {
  EXPECT_EQ(manim_cpp::renderer::to_string(manim_cpp::renderer::RendererType::kCairo),
            std::string("cairo"));
  EXPECT_EQ(
      manim_cpp::renderer::to_string(manim_cpp::renderer::RendererType::kOpenGL),
      std::string("opengl"));
}

TEST(Renderer, CairoRendererGeneratesDeterministicFrameFileNames) {
  manim_cpp::renderer::CairoRenderer renderer;

  EXPECT_EQ(renderer.frame_file_name("DemoScene", 1),
            std::string("DemoScene_000001.png"));
  EXPECT_EQ(renderer.frame_file_name("DemoScene", 42),
            std::string("DemoScene_000042.png"));
}

TEST(Renderer, CairoRendererSkipsDuplicateStaticFrameSignatures) {
  manim_cpp::renderer::CairoRenderer renderer;

  EXPECT_TRUE(renderer.should_render_for_signature("frame-hash-a"));
  EXPECT_FALSE(renderer.should_render_for_signature("frame-hash-a"));
  EXPECT_TRUE(renderer.should_render_for_signature("frame-hash-b"));

  renderer.reset_frame_cache();
  EXPECT_TRUE(renderer.should_render_for_signature("frame-hash-a"));
}
