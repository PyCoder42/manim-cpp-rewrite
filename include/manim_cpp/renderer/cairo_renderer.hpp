#pragma once

#include <string>

#include "manim_cpp/renderer/renderer.hpp"

namespace manim_cpp::renderer {

class CairoRenderer final : public Renderer {
 public:
  RendererType type() const override { return RendererType::kCairo; }
  std::string name() const override { return "cairo"; }
};

}  // namespace manim_cpp::renderer
