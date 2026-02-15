#include "manim_cpp/renderer/renderer.hpp"

#include <algorithm>
#include <cctype>
#include <memory>
#include <utility>

#include "manim_cpp/renderer/cairo_renderer.hpp"
#include "manim_cpp/renderer/opengl_renderer.hpp"

namespace manim_cpp::renderer {
namespace {

std::string lower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](const unsigned char ch) {
                   return static_cast<char>(std::tolower(ch));
                 });
  return value;
}

}  // namespace

std::string to_string(const RendererType type) {
  switch (type) {
    case RendererType::kCairo:
      return "cairo";
    case RendererType::kOpenGL:
      return "opengl";
  }
  return "unknown";
}

std::optional<RendererType> parse_renderer_type(std::string renderer_name) {
  renderer_name = lower(std::move(renderer_name));
  if (renderer_name == "cairo") {
    return RendererType::kCairo;
  }
  if (renderer_name == "opengl") {
    return RendererType::kOpenGL;
  }
  return std::nullopt;
}

std::unique_ptr<Renderer> make_renderer(const RendererType type) {
  switch (type) {
    case RendererType::kCairo:
      return std::make_unique<CairoRenderer>();
    case RendererType::kOpenGL:
      return std::make_unique<OpenGLRenderer>();
  }
  return nullptr;
}

}  // namespace manim_cpp::renderer
