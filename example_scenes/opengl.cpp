#include <memory>

#include "manim_cpp/animation/basic_animations.hpp"
#include "manim_cpp/mobject/geometry.hpp"
#include "manim_cpp/scene/registry.hpp"
#include "manim_cpp/scene/scene.hpp"

using namespace manim_cpp::scene;
using manim_cpp::animation::FadeToOpacityAnimation;
using manim_cpp::animation::MoveToAnimation;
using manim_cpp::animation::ShiftAnimation;
using manim_cpp::math::Vec3;
using manim_cpp::mobject::Circle;
using manim_cpp::mobject::Dot;
using manim_cpp::mobject::Square;

namespace {

void run_opengl_stub(Scene& scene,
                     const Vec3& start,
                     const Vec3& drift,
                     const Vec3& marker_target,
                     const double marker_opacity) {
  auto body = std::make_shared<Square>(0.9);
  body->move_to(start);
  auto marker = std::make_shared<Dot>(0.08);
  marker->set_opacity(0.0);
  marker->move_to(start);
  auto halo = std::make_shared<Circle>(0.55);
  halo->move_to(start);
  halo->set_opacity(0.2);
  scene.add(body);
  scene.add(marker);
  scene.add(halo);

  ShiftAnimation drift_body(body, drift);
  drift_body.set_run_time_seconds(0.45);
  scene.play(drift_body, 8);

  MoveToAnimation move_marker(marker, marker_target);
  move_marker.set_run_time_seconds(0.45);
  scene.play(move_marker, 8);

  FadeToOpacityAnimation reveal_marker(marker, marker_opacity);
  reveal_marker.set_run_time_seconds(0.35);
  scene.play(reveal_marker, 7);
}

}  // namespace

class TextTest : public Scene {
 public:
  std::string scene_name() const override { return "TextTest"; }
  void construct() override {
    run_opengl_stub(*this, Vec3{-1.0, 0.0, 0.0}, Vec3{1.2, 0.0, 0.0},
                    Vec3{0.8, 0.6, 0.0}, 0.85);
  }
};
MANIM_REGISTER_SCENE(TextTest);

class GuiTest : public Scene {
 public:
  std::string scene_name() const override { return "GuiTest"; }
  void construct() override {
    run_opengl_stub(*this, Vec3{0.0, 0.0, 0.0}, Vec3{0.0, 0.8, 0.0},
                    Vec3{-0.6, 0.9, 0.0}, 0.8);
  }
};
MANIM_REGISTER_SCENE(GuiTest);

class GuiTest2 : public Scene {
 public:
  std::string scene_name() const override { return "GuiTest2"; }
  void construct() override {
    run_opengl_stub(*this, Vec3{0.0, -0.5, 0.0}, Vec3{0.9, 0.7, 0.0},
                    Vec3{1.0, -0.2, 0.0}, 0.8);
  }
};
MANIM_REGISTER_SCENE(GuiTest2);

class ThreeDMobjectTest : public Scene {
 public:
  std::string scene_name() const override { return "ThreeDMobjectTest"; }
  void construct() override {
    run_opengl_stub(*this, Vec3{-0.8, -0.3, 0.0}, Vec3{1.6, 0.0, 0.0},
                    Vec3{0.0, 1.0, 0.0}, 0.9);
  }
};
MANIM_REGISTER_SCENE(ThreeDMobjectTest);

class NamedFullScreenQuad : public Scene {
 public:
  std::string scene_name() const override { return "NamedFullScreenQuad"; }
  void construct() override {
    run_opengl_stub(*this, Vec3{0.0, 0.0, 0.0}, Vec3{0.4, 0.4, 0.0},
                    Vec3{-0.7, 0.5, 0.0}, 0.75);
  }
};
MANIM_REGISTER_SCENE(NamedFullScreenQuad);

class InlineFullScreenQuad : public Scene {
 public:
  std::string scene_name() const override { return "InlineFullScreenQuad"; }
  void construct() override {
    run_opengl_stub(*this, Vec3{0.4, -0.2, 0.0}, Vec3{-0.9, 0.6, 0.0},
                    Vec3{-1.0, -0.4, 0.0}, 0.7);
  }
};
MANIM_REGISTER_SCENE(InlineFullScreenQuad);

class SimpleInlineFullScreenQuad : public Scene {
 public:
  std::string scene_name() const override { return "SimpleInlineFullScreenQuad"; }
  void construct() override {
    run_opengl_stub(*this, Vec3{-0.2, 0.2, 0.0}, Vec3{0.5, -0.8, 0.0},
                    Vec3{0.9, -0.6, 0.0}, 0.8);
  }
};
MANIM_REGISTER_SCENE(SimpleInlineFullScreenQuad);

class InlineShaderExample : public Scene {
 public:
  std::string scene_name() const override { return "InlineShaderExample"; }
  void construct() override {
    run_opengl_stub(*this, Vec3{-0.5, 0.4, 0.0}, Vec3{1.0, -0.3, 0.0},
                    Vec3{0.0, 0.0, 0.0}, 0.95);
  }
};
MANIM_REGISTER_SCENE(InlineShaderExample);

class NamedShaderExample : public Scene {
 public:
  std::string scene_name() const override { return "NamedShaderExample"; }
  void construct() override {
    run_opengl_stub(*this, Vec3{0.7, 0.2, 0.0}, Vec3{-1.2, 0.2, 0.0},
                    Vec3{-0.3, 0.9, 0.0}, 0.85);
  }
};
MANIM_REGISTER_SCENE(NamedShaderExample);

class InteractiveDevelopment : public Scene {
 public:
  std::string scene_name() const override { return "InteractiveDevelopment"; }
  void construct() override {
    run_opengl_stub(*this, Vec3{0.0, 0.0, 0.0}, Vec3{0.0, 1.0, 0.0},
                    Vec3{0.0, -1.0, 0.0}, 1.0);
  }
};
MANIM_REGISTER_SCENE(InteractiveDevelopment);

class SurfaceExample : public Scene {
 public:
  std::string scene_name() const override { return "SurfaceExample"; }
  void construct() override {
    run_opengl_stub(*this, Vec3{-0.4, 0.0, 0.0}, Vec3{0.8, 0.3, 0.0},
                    Vec3{1.1, 0.7, 0.0}, 0.9);
  }
};
MANIM_REGISTER_SCENE(SurfaceExample);
