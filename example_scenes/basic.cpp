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

class OpeningManim : public Scene {
 public:
  std::string scene_name() const override { return "OpeningManim"; }
  void construct() override {
    auto dot = std::make_shared<Dot>(0.14);
    add(dot);

    MoveToAnimation move(dot, Vec3{2.0, 0.0, 0.0});
    move.set_run_time_seconds(0.8);
    play(move, 12);

    FadeToOpacityAnimation fade(dot, 0.2);
    fade.set_run_time_seconds(0.4);
    play(fade, 8);
  }
};
MANIM_REGISTER_SCENE(OpeningManim);

class SquareToCircle : public Scene {
 public:
  std::string scene_name() const override { return "SquareToCircle"; }
  void construct() override {
    auto square = std::make_shared<Square>(2.0);
    add(square);

    ShiftAnimation shift(square, Vec3{1.5, 0.0, 0.0});
    shift.set_run_time_seconds(0.6);
    play(shift, 10);

    auto circle = std::make_shared<Circle>(1.0);
    circle->move_to(square->center());
    circle->set_opacity(0.0);
    add(circle);

    FadeToOpacityAnimation fade_in_circle(circle, 1.0);
    fade_in_circle.set_run_time_seconds(0.4);
    play(fade_in_circle, 8);

    FadeToOpacityAnimation fade_out_square(square, 0.0);
    fade_out_square.set_run_time_seconds(0.4);
    play(fade_out_square, 8);
  }
};
MANIM_REGISTER_SCENE(SquareToCircle);

class WarpSquare : public Scene {
 public:
  std::string scene_name() const override { return "WarpSquare"; }
  void construct() override {
    auto square = std::make_shared<Square>(1.5);
    add(square);

    MoveToAnimation jump_one(square, Vec3{-2.0, 1.5, 0.0});
    jump_one.set_run_time_seconds(0.5);
    play(jump_one, 8);

    MoveToAnimation jump_two(square, Vec3{2.0, -1.5, 0.0});
    jump_two.set_run_time_seconds(0.5);
    play(jump_two, 8);
  }
};
MANIM_REGISTER_SCENE(WarpSquare);

class WriteStuff : public Scene {
 public:
  std::string scene_name() const override { return "WriteStuff"; }
  void construct() override {
    auto dot = std::make_shared<Dot>(0.1);
    auto circle = std::make_shared<Circle>(1.2);
    circle->set_opacity(0.0);
    add(dot);
    add(circle);

    FadeToOpacityAnimation reveal_circle(circle, 0.6);
    reveal_circle.set_run_time_seconds(0.5);
    play(reveal_circle, 10);

    MoveToAnimation move_dot(dot, Vec3{0.0, 1.2, 0.0});
    move_dot.set_run_time_seconds(0.6);
    play(move_dot, 12);
  }
};
MANIM_REGISTER_SCENE(WriteStuff);

class UpdatersExample : public Scene {
 public:
  std::string scene_name() const override { return "UpdatersExample"; }
  void construct() override {
    auto dot = std::make_shared<Dot>(0.08);
    add(dot);

    add_updater([dot](const double delta_seconds) {
      dot->shift(Vec3{delta_seconds, 0.5 * delta_seconds, 0.0});
    });

    FadeToOpacityAnimation hold(dot, dot->opacity());
    hold.set_run_time_seconds(1.0);
    play(hold, 20);
    clear_updaters();
  }
};
MANIM_REGISTER_SCENE(UpdatersExample);

class SpiralInExample : public Scene {
 public:
  std::string scene_name() const override { return "SpiralInExample"; }
  void construct() override {
    auto dot = std::make_shared<Dot>(0.08);
    dot->move_to(Vec3{2.0, 0.0, 0.0});
    add(dot);

    MoveToAnimation move_mid(dot, Vec3{1.0, 1.0, 0.0});
    move_mid.set_run_time_seconds(0.4);
    play(move_mid, 8);

    MoveToAnimation move_center(dot, Vec3{0.0, 0.0, 0.0});
    move_center.set_run_time_seconds(0.4);
    play(move_center, 8);
  }
};
MANIM_REGISTER_SCENE(SpiralInExample);

class LineJoints : public Scene {
 public:
  std::string scene_name() const override { return "LineJoints"; }
  void construct() override {
    auto first = std::make_shared<Square>(0.8);
    auto second = std::make_shared<Square>(0.8);
    second->move_to(Vec3{1.2, 0.0, 0.0});
    add(first);
    add(second);

    ShiftAnimation join(second, Vec3{-0.4, 0.0, 0.0});
    join.set_run_time_seconds(0.5);
    play(join, 10);
  }
};
MANIM_REGISTER_SCENE(LineJoints);
