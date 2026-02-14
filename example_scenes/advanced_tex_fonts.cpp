#include "manim_cpp/scene/registry.hpp"
#include "manim_cpp/scene/scene.hpp"

using namespace manim_cpp::scene;

class TexFontTemplateManual : public Scene {
 public:
  std::string scene_name() const override { return "TexFontTemplateManual"; }
  void construct() override {
    // TODO(example_scenes): Port original TeX font template example from advanced_tex_fonts.py.
  }
};
MANIM_REGISTER_SCENE(TexFontTemplateManual);

class TexFontTemplateLibrary : public Scene {
 public:
  std::string scene_name() const override { return "TexFontTemplateLibrary"; }
  void construct() override {
    // TODO(example_scenes): Port template-library iteration example from advanced_tex_fonts.py.
  }
};
MANIM_REGISTER_SCENE(TexFontTemplateLibrary);
