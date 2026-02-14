#include "manim_cpp/scene/registry.hpp"
#include "manim_cpp/scene/scene.hpp"

using namespace manim_cpp::scene;

class OpeningManim : public Scene {
 public:
  std::string scene_name() const override { return "OpeningManim"; }
  void construct() override {
    // TODO(example_scenes): Port original Python scene choreography from basic.py.
  }
};
MANIM_REGISTER_SCENE(OpeningManim);

class SquareToCircle : public Scene {
 public:
  std::string scene_name() const override { return "SquareToCircle"; }
  void construct() override {
    // TODO(example_scenes): Port original Python scene choreography from basic.py.
  }
};
MANIM_REGISTER_SCENE(SquareToCircle);

class WarpSquare : public Scene {
 public:
  std::string scene_name() const override { return "WarpSquare"; }
  void construct() override {
    // TODO(example_scenes): Port original Python scene choreography from basic.py.
  }
};
MANIM_REGISTER_SCENE(WarpSquare);

class WriteStuff : public Scene {
 public:
  std::string scene_name() const override { return "WriteStuff"; }
  void construct() override {
    // TODO(example_scenes): Port original Python scene choreography from basic.py.
  }
};
MANIM_REGISTER_SCENE(WriteStuff);

class UpdatersExample : public Scene {
 public:
  std::string scene_name() const override { return "UpdatersExample"; }
  void construct() override {
    // TODO(example_scenes): Port original Python scene choreography from basic.py.
  }
};
MANIM_REGISTER_SCENE(UpdatersExample);

class SpiralInExample : public Scene {
 public:
  std::string scene_name() const override { return "SpiralInExample"; }
  void construct() override {
    // TODO(example_scenes): Port original Python scene choreography from basic.py.
  }
};
MANIM_REGISTER_SCENE(SpiralInExample);

class LineJoints : public Scene {
 public:
  std::string scene_name() const override { return "LineJoints"; }
  void construct() override {
    // TODO(example_scenes): Port original Python scene choreography from basic.py.
  }
};
MANIM_REGISTER_SCENE(LineJoints);
