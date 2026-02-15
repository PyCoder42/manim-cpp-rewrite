#include "manim_cpp/scene/registry.hpp"
#include "manim_cpp/scene/scene.hpp"

using namespace manim_cpp::scene;

class TextTest : public Scene {
 public:
  std::string scene_name() const override { return "TextTest"; }
  void construct() override {
    // TODO(example_scenes): Port OpenGL text example from opengl.py.
  }
};
MANIM_REGISTER_SCENE(TextTest);

class GuiTest : public Scene {
 public:
  std::string scene_name() const override { return "GuiTest"; }
  void construct() override {
    // TODO(example_scenes): Port OpenGL GUI example from opengl.py.
  }
};
MANIM_REGISTER_SCENE(GuiTest);

class GuiTest2 : public Scene {
 public:
  std::string scene_name() const override { return "GuiTest2"; }
  void construct() override {
    // TODO(example_scenes): Port OpenGL GUI widget example from opengl.py.
  }
};
MANIM_REGISTER_SCENE(GuiTest2);

class ThreeDMobjectTest : public Scene {
 public:
  std::string scene_name() const override { return "ThreeDMobjectTest"; }
  void construct() override {
    // TODO(example_scenes): Port 3D mobject OpenGL example from opengl.py.
  }
};
MANIM_REGISTER_SCENE(ThreeDMobjectTest);

class NamedFullScreenQuad : public Scene {
 public:
  std::string scene_name() const override { return "NamedFullScreenQuad"; }
  void construct() override {
    // TODO(example_scenes): Port named fullscreen shader example from opengl.py.
  }
};
MANIM_REGISTER_SCENE(NamedFullScreenQuad);

class InlineFullScreenQuad : public Scene {
 public:
  std::string scene_name() const override { return "InlineFullScreenQuad"; }
  void construct() override {
    // TODO(example_scenes): Port inline fullscreen shader example from opengl.py.
  }
};
MANIM_REGISTER_SCENE(InlineFullScreenQuad);

class SimpleInlineFullScreenQuad : public Scene {
 public:
  std::string scene_name() const override { return "SimpleInlineFullScreenQuad"; }
  void construct() override {
    // TODO(example_scenes): Port simple inline shader example from opengl.py.
  }
};
MANIM_REGISTER_SCENE(SimpleInlineFullScreenQuad);

class InlineShaderExample : public Scene {
 public:
  std::string scene_name() const override { return "InlineShaderExample"; }
  void construct() override {
    // TODO(example_scenes): Port inline shader scene from opengl.py.
  }
};
MANIM_REGISTER_SCENE(InlineShaderExample);

class NamedShaderExample : public Scene {
 public:
  std::string scene_name() const override { return "NamedShaderExample"; }
  void construct() override {
    // TODO(example_scenes): Port named shader scene from opengl.py.
  }
};
MANIM_REGISTER_SCENE(NamedShaderExample);

class InteractiveDevelopment : public Scene {
 public:
  std::string scene_name() const override { return "InteractiveDevelopment"; }
  void construct() override {
    // TODO(example_scenes): Port interactive development scene from opengl.py.
  }
};
MANIM_REGISTER_SCENE(InteractiveDevelopment);

class SurfaceExample : public Scene {
 public:
  std::string scene_name() const override { return "SurfaceExample"; }
  void construct() override {
    // TODO(example_scenes): Port OpenGL surface example from opengl.py.
  }
};
MANIM_REGISTER_SCENE(SurfaceExample);
