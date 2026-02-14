#pragma once

#include <memory>
#include <optional>
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

std::string to_string(RendererType type);
std::optional<RendererType> parse_renderer_type(std::string renderer_name);
std::unique_ptr<Renderer> make_renderer(RendererType type);

}  // namespace manim_cpp::renderer
