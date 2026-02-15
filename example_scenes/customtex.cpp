#include <memory>

#include "manim_cpp/animation/basic_animations.hpp"
#include "manim_cpp/mobject/geometry.hpp"
#include "manim_cpp/scene/registry.hpp"
#include "manim_cpp/scene/scene.hpp"

using namespace manim_cpp::scene;
using manim_cpp::animation::FadeToOpacityAnimation;
using manim_cpp::animation::ShiftAnimation;
using manim_cpp::math::Vec3;
using manim_cpp::mobject::Circle;
using manim_cpp::mobject::Dot;

class TexTemplateFromCLI : public Scene {
 public:
  std::string scene_name() const override { return "TexTemplateFromCLI"; }
  void construct() override {
    auto anchor = std::make_shared<Dot>(0.09);
    auto envelope = std::make_shared<Circle>(1.1);
    envelope->set_opacity(0.3);
    add(anchor);
    add(envelope);

    ShiftAnimation nudge(anchor, Vec3{1.2, 0.4, 0.0});
    nudge.set_run_time_seconds(0.6);
    play(nudge, 12);
  }
};
MANIM_REGISTER_SCENE(TexTemplateFromCLI);

class InCodeTexTemplate : public Scene {
 public:
  std::string scene_name() const override { return "InCodeTexTemplate"; }
  void construct() override {
    auto inner = std::make_shared<Circle>(0.55);
    inner->set_opacity(0.0);
    add(inner);

    FadeToOpacityAnimation reveal(inner, 0.9);
    reveal.set_run_time_seconds(0.45);
    play(reveal, 9);
  }
};
MANIM_REGISTER_SCENE(InCodeTexTemplate);
