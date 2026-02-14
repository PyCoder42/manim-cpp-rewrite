#include "manim_cpp/scene/registry.hpp"
#include "manim_cpp/scene/scene.hpp"

using namespace manim_cpp::scene;

class TexTemplateFromCLI : public Scene {
 public:
  std::string scene_name() const override { return "TexTemplateFromCLI"; }
  void construct() override {
    // TODO(example_scenes): Port custom template example from customtex.py.
  }
};
MANIM_REGISTER_SCENE(TexTemplateFromCLI);

class InCodeTexTemplate : public Scene {
 public:
  std::string scene_name() const override { return "InCodeTexTemplate"; }
  void construct() override {
    // TODO(example_scenes): Port in-code template customization example from customtex.py.
  }
};
MANIM_REGISTER_SCENE(InCodeTexTemplate);
