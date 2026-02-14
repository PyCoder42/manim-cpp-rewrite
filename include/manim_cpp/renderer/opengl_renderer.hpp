#pragma once

#include <string>

#include "manim_cpp/renderer/renderer.hpp"

namespace manim_cpp::renderer {

class OpenGLRenderer final : public Renderer {
 public:
  RendererType type() const override { return RendererType::kOpenGL; }
  std::string name() const override { return "opengl"; }
};

}  // namespace manim_cpp::renderer
