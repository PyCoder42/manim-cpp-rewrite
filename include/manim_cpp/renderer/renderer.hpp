#pragma once

#include <string>

namespace manim_cpp::renderer {

enum class RendererType {
  kCairo,
  kOpenGL,
};

class Renderer {
 public:
  virtual ~Renderer() = default;
  virtual RendererType type() const = 0;
  virtual std::string name() const = 0;
};

}  // namespace manim_cpp::renderer
