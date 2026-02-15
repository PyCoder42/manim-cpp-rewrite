#include <memory>

#include "manim_cpp/animation/basic_animations.hpp"
#include "manim_cpp/mobject/geometry.hpp"
#include "manim_cpp/scene/registry.hpp"
#include "manim_cpp/scene/scene.hpp"

using namespace manim_cpp::scene;
using manim_cpp::animation::FadeToOpacityAnimation;
using manim_cpp::animation::MoveToAnimation;
using manim_cpp::math::Vec3;
using manim_cpp::mobject::Circle;
using manim_cpp::mobject::Square;

class TexFontTemplateManual : public Scene {
 public:
  std::string scene_name() const override { return "TexFontTemplateManual"; }
  void construct() override {
    auto glyph_block = std::make_shared<Square>(1.4);
    auto accent_block = std::make_shared<Square>(0.7);
    accent_block->set_opacity(0.0);
    accent_block->move_to(Vec3{-1.5, 0.0, 0.0});
    add(glyph_block);
    add(accent_block);

    MoveToAnimation center_shift(glyph_block, Vec3{1.0, 0.0, 0.0});
    center_shift.set_run_time_seconds(0.55);
    play(center_shift, 10);

    FadeToOpacityAnimation reveal(accent_block, 0.9);
    reveal.set_run_time_seconds(0.4);
    play(reveal, 8);
  }
};
MANIM_REGISTER_SCENE(TexFontTemplateManual);

class TexFontTemplateLibrary : public Scene {
 public:
  std::string scene_name() const override { return "TexFontTemplateLibrary"; }
  void construct() override {
    auto primary = std::make_shared<Circle>(0.8);
    auto secondary = std::make_shared<Circle>(0.45);
    secondary->set_opacity(0.0);
    secondary->move_to(Vec3{1.4, 0.0, 0.0});
    add(primary);
    add(secondary);

    MoveToAnimation arc(primary, Vec3{-1.0, 0.6, 0.0});
    arc.set_run_time_seconds(0.5);
    play(arc, 10);

    FadeToOpacityAnimation reveal_secondary(secondary, 0.75);
    reveal_secondary.set_run_time_seconds(0.5);
    play(reveal_secondary, 10);
  }
};
MANIM_REGISTER_SCENE(TexFontTemplateLibrary);
