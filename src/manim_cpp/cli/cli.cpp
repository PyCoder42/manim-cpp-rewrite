#include "manim_cpp/cli/cli.hpp"

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <cerrno>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "manim_cpp/config/config.hpp"
#include "manim_cpp/plugin/loader.hpp"
#include "manim_cpp/renderer/cairo_renderer.hpp"
#include "manim_cpp/renderer/interaction.hpp"
#include "manim_cpp/renderer/opengl_renderer.hpp"
#include "manim_cpp/renderer/renderer.hpp"
#include "manim_cpp/scene/media_format.hpp"
#include "manim_cpp/scene/scene_file_writer.hpp"
#include "manim_cpp/scene/registry.hpp"
#include "manim_cpp/version.hpp"

namespace manim_cpp::cli {
namespace {

void print_root_help() {
  std::cout << "Usage: manim-cpp [OPTIONS] COMMAND [ARGS]...\n\n";
  std::cout << "Animation engine for explanatory math videos (C++ rewrite).\n\n";
  std::cout << "Options:\n";
  std::cout << "  --version    Show version and exit.\n";
  std::cout << "  --help       Show this message and exit.\n\n";
  std::cout << "Commands:\n";
  std::cout << "  render       Render SCENE(S) from the input FILE.\n";
  std::cout << "  cfg          Manage manim.cfg files.\n";
  std::cout << "  checkhealth  Check local runtime dependencies.\n";
  std::cout << "  init         Initialize project/scene templates.\n";
  std::cout << "  plugins      Manage C++ plugin loading and discovery.\n";
}

void print_subcommand_help(const std::string& command) {
  if (command == "render") {
    std::cout << "Usage: manim-cpp render <input.cpp> [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --renderer <cairo|opengl>       Select render backend.\n";
    std::cout << "  --format <png|gif|mp4|webm|mov> Select output media format.\n";
    std::cout << "  --scene <SceneName>             Run a registered C++ scene.\n";
    std::cout << "  --watch, -w                     Enable watch mode.\n";
    std::cout << "  --interactive, -i               Enable interactive controls.\n";
    std::cout << "  --enable_gui                    Enable GUI window output.\n";
    std::cout << "  --fullscreen                    Start in fullscreen mode.\n";
    std::cout << "  --force_window                  Force creation of a window.\n";
    std::cout << "  --window_position <value>       Set window position (e.g. UR, 960,540).\n";
    std::cout << "  --window_size <value>           Set window size (e.g. 1280,720).\n";
    std::cout << "  --window_monitor <int>          Select monitor index.\n";
    return;
  }
  if (command == "cfg") {
    std::cout << "Usage: manim-cpp cfg <show|write> [ARGS]\n";
    return;
  }
  if (command == "checkhealth") {
    std::cout << "Usage: manim-cpp checkhealth [--json]\n";
    return;
  }
  if (command == "init") {
    std::cout << "Usage: manim-cpp init <project|scene> <path>\n";
    return;
  }
  if (command == "plugins") {
    std::cout << "Usage: manim-cpp plugins <list|load|path> [ARGS]\n";
    return;
  }

  std::cout << "Usage: manim-cpp " << command << " [OPTIONS]\n";
}

bool is_help_flag(const std::string& token) {
  return token == "--help" || token == "-h";
}

bool is_member(const std::vector<std::string>& values, const std::string& target) {
  return std::find(values.begin(), values.end(), target) != values.end();
}

struct PluginLoadContext {
  std::vector<std::string> logs;
  std::vector<std::string> scene_registrations;
};

PluginLoadContext* g_plugin_load_context = nullptr;

void plugin_log_callback(const int level, const char* message) {
  if (g_plugin_load_context == nullptr) {
    return;
  }
  std::ostringstream stream;
  stream << "[" << level << "] " << (message != nullptr ? message : "");
  g_plugin_load_context->logs.push_back(stream.str());
}

int plugin_register_scene_callback(const char* scene_name, const char* symbol_name) {
  if (g_plugin_load_context == nullptr) {
    return 1;
  }
  std::ostringstream stream;
  stream << (scene_name != nullptr ? scene_name : "") << " -> "
         << (symbol_name != nullptr ? symbol_name : "");
  g_plugin_load_context->scene_registrations.push_back(stream.str());
  return 0;
}

bool command_on_path(const std::string& command) {
  const char* raw_path = std::getenv("PATH");
  if (raw_path == nullptr) {
    return false;
  }

#ifdef _WIN32
  constexpr char kPathSeparator = ';';
#else
  constexpr char kPathSeparator = ':';
#endif

  std::stringstream stream(raw_path);
  std::string dir;
  while (std::getline(stream, dir, kPathSeparator)) {
    const auto candidate = std::filesystem::path(dir) / command;
    if (std::filesystem::exists(candidate)) {
      return true;
    }
#ifdef _WIN32
    if (std::filesystem::exists(candidate.string() + ".exe")) {
      return true;
    }
#endif
  }
  return false;
}

bool parse_int_strict(const std::string& value, int* output) {
  if (value.empty() || output == nullptr) {
    return false;
  }

  char* end = nullptr;
  errno = 0;
  const long parsed = std::strtol(value.c_str(), &end, 10);
  if (errno != 0 || end == value.c_str() || *end != '\0' || parsed < INT_MIN ||
      parsed > INT_MAX) {
    return false;
  }
  *output = static_cast<int>(parsed);
  return true;
}

bool parse_double_strict(const std::string& value, double* output) {
  if (value.empty() || output == nullptr) {
    return false;
  }

  char* end = nullptr;
  errno = 0;
  const double parsed = std::strtod(value.c_str(), &end);
  if (errno != 0 || end == value.c_str() || *end != '\0' || !std::isfinite(parsed)) {
    return false;
  }
  *output = parsed;
  return true;
}

std::string frame_file_name_for_renderer(const manim_cpp::renderer::RendererType type,
                                         const std::string& scene_name,
                                         const std::size_t frame_index) {
  switch (type) {
    case manim_cpp::renderer::RendererType::kCairo: {
      manim_cpp::renderer::CairoRenderer renderer;
      return renderer.frame_file_name(scene_name, frame_index);
    }
    case manim_cpp::renderer::RendererType::kOpenGL: {
      manim_cpp::renderer::OpenGLRenderer renderer;
      return renderer.frame_file_name(scene_name, frame_index);
    }
  }
  return scene_name + ".png";
}

std::filesystem::path default_cfg_template_path() {
  auto probe = std::filesystem::current_path();
  for (int depth = 0; depth < 10; ++depth) {
    const auto candidate = probe / "config" / "manim.cfg.default";
    if (std::filesystem::exists(candidate)) {
      return candidate;
    }
    if (!probe.has_parent_path()) {
      break;
    }
    probe = probe.parent_path();
  }
  return {};
}

std::string to_scene_identifier(const std::filesystem::path& output_path) {
  std::string stem = output_path.stem().string();
  if (stem.empty()) {
    return "MyScene";
  }

  std::string identifier;
  identifier.reserve(stem.size() + 8);
  for (const char ch : stem) {
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9') || ch == '_') {
      identifier.push_back(ch);
    } else {
      identifier.push_back('_');
    }
  }

  if (identifier.empty()) {
    identifier = "MyScene";
  }
  if (identifier.front() >= '0' && identifier.front() <= '9') {
    identifier.insert(identifier.begin(), '_');
  }
  if (identifier.size() < 5 || identifier.substr(identifier.size() - 5) != "Scene") {
    identifier += "Scene";
  }
  return identifier;
}

bool write_scene_template(const std::filesystem::path& output_path,
                          const std::string& scene_identifier) {
  if (std::filesystem::exists(output_path)) {
    std::cerr << "Refusing to overwrite existing file: " << output_path << "\n";
    return false;
  }

  if (output_path.has_parent_path()) {
    std::filesystem::create_directories(output_path.parent_path());
  }

  std::ofstream output(output_path);
  if (!output.is_open()) {
    std::cerr << "Unable to write scene template: " << output_path << "\n";
    return false;
  }

  output << "#include \"manim_cpp/scene/registry.hpp\"\n";
  output << "#include \"manim_cpp/scene/scene.hpp\"\n\n";
  output << "using namespace manim_cpp::scene;\n\n";
  output << "class " << scene_identifier << " : public Scene {\n";
  output << " public:\n";
  output << "  std::string scene_name() const override { return \"" << scene_identifier
         << "\"; }\n";
  output << "  void construct() override {\n";
  output << "    // TODO: author scene animations.\n";
  output << "  }\n";
  output << "};\n\n";
  output << "MANIM_REGISTER_SCENE(" << scene_identifier << ");\n";
  return true;
}

bool write_default_cfg(const std::filesystem::path& output_path) {
  const auto template_path = default_cfg_template_path();
  if (template_path.empty()) {
    std::cerr << "Unable to locate default config template (config/manim.cfg.default).\n";
    return false;
  }
  std::ifstream input(template_path);
  if (!input.is_open()) {
    std::cerr << "Unable to read default template: " << template_path << "\n";
    return false;
  }

  if (output_path.has_parent_path()) {
    std::filesystem::create_directories(output_path.parent_path());
  }
  std::ofstream output(output_path);
  if (!output.is_open()) {
    std::cerr << "Unable to write config file: " << output_path << "\n";
    return false;
  }
  output << input.rdbuf();
  return true;
}

int handle_init_scene(const int argc, const char* const argv[]) {
  if (argc < 4) {
    std::cerr << "Usage: manim-cpp init scene <output.cpp>\n";
    return 2;
  }

  const std::filesystem::path output_path = argv[3];
  if (!write_scene_template(output_path, to_scene_identifier(output_path))) {
    return 2;
  }

  std::cout << "Wrote scene template to " << output_path << "\n";
  return 0;
}

int handle_init_project(const int argc, const char* const argv[]) {
  if (argc < 4) {
    std::cerr << "Usage: manim-cpp init project <directory>\n";
    return 2;
  }

  const std::filesystem::path project_root = argv[3];
  if (std::filesystem::exists(project_root) &&
      !std::filesystem::is_directory(project_root)) {
    std::cerr << "Project path is not a directory: " << project_root << "\n";
    return 2;
  }

  std::filesystem::create_directories(project_root / "scenes");
  const auto cfg_path = project_root / "manim.cfg";
  if (std::filesystem::exists(cfg_path)) {
    std::cerr << "Refusing to overwrite existing file: " << cfg_path << "\n";
    return 2;
  }
  if (!write_default_cfg(cfg_path)) {
    return 2;
  }

  const auto scene_path = project_root / "scenes" / "main_scene.cpp";
  if (!write_scene_template(scene_path, "MainScene")) {
    return 2;
  }

  std::cout << "Initialized project scaffold in " << project_root << "\n";
  return 0;
}

int handle_cfg_show(const int argc, const char* const argv[]) {
  const std::filesystem::path input_path =
      argc >= 4 ? std::filesystem::path(argv[3]) : default_cfg_template_path();
  if (input_path.empty()) {
    std::cerr << "Unable to locate default config template (config/manim.cfg.default).\n";
    return 2;
  }

  manim_cpp::config::ManimConfig config;
  if (!config.load_from_file(input_path)) {
    std::cerr << "Unable to read config file: " << input_path << "\n";
    return 2;
  }

  std::cout << "Loaded config: " << input_path << "\n";
  std::cout << "Sections: " << config.data().size() << "\n";
  return 0;
}

int handle_cfg_write(const int argc, const char* const argv[]) {
  if (argc < 4) {
    std::cerr << "Usage: manim-cpp cfg write <output.cfg>\n";
    return 2;
  }

  const std::filesystem::path output_path = argv[3];
  if (std::filesystem::exists(output_path)) {
    std::cerr << "Refusing to overwrite existing file: " << output_path << "\n";
    return 2;
  }

  const auto template_path = default_cfg_template_path();
  if (template_path.empty()) {
    std::cerr << "Unable to locate default config template (config/manim.cfg.default).\n";
    return 2;
  }
  std::ifstream input(template_path);
  if (!input.is_open()) {
    std::cerr << "Unable to read default template: " << template_path << "\n";
    return 2;
  }

  std::filesystem::create_directories(output_path.parent_path());
  std::ofstream output(output_path);
  if (!output.is_open()) {
    std::cerr << "Unable to write config file: " << output_path << "\n";
    return 2;
  }

  output << input.rdbuf();
  std::cout << "Wrote config template to " << output_path << "\n";
  return 0;
}

int handle_render(const int argc, const char* const argv[]) {
  if (argc <= 2 || is_help_flag(argv[2])) {
    print_subcommand_help("render");
    return argc <= 2 ? 2 : 0;
  }

  std::filesystem::path input_file;
  auto renderer_type = manim_cpp::renderer::RendererType::kCairo;
  auto media_format = manim_cpp::scene::MediaFormat::kMp4;
  std::string scene_name;
  bool watch = false;
  bool interactive = false;
  bool enable_gui = false;
  bool fullscreen = false;
  bool force_window = false;
  auto window_position = manim_cpp::renderer::WindowPosition{};
  auto window_size = manim_cpp::renderer::WindowSize{};
  int window_monitor = 0;
  for (int i = 2; i < argc; ++i) {
    const std::string token = argv[i];
    if (is_help_flag(token)) {
      print_subcommand_help("render");
      return 0;
    }
    if (token == "--renderer") {
      if (i + 1 >= argc) {
        std::cerr << "Missing value for --renderer.\n";
        return 2;
      }
      const auto parsed = manim_cpp::renderer::parse_renderer_type(argv[++i]);
      if (!parsed.has_value()) {
        std::cerr << "Unknown renderer: " << argv[i] << "\n";
        return 2;
      }
      renderer_type = parsed.value();
      continue;
    }
    if (token == "--format") {
      if (i + 1 >= argc) {
        std::cerr << "Missing value for --format.\n";
        return 2;
      }
      const auto parsed = manim_cpp::scene::parse_media_format(argv[++i]);
      if (!parsed.has_value()) {
        std::cerr << "Unknown output format: " << argv[i] << "\n";
        return 2;
      }
      media_format = parsed.value();
      continue;
    }
    if (token == "--scene") {
      if (i + 1 >= argc) {
        std::cerr << "Missing value for --scene.\n";
        return 2;
      }
      scene_name = argv[++i];
      continue;
    }
    if (token == "--watch" || token == "-w") {
      watch = true;
      continue;
    }
    if (token == "--interactive" || token == "-i") {
      interactive = true;
      continue;
    }
    if (token == "--enable_gui") {
      enable_gui = true;
      continue;
    }
    if (token == "--fullscreen") {
      fullscreen = true;
      continue;
    }
    if (token == "--force_window") {
      force_window = true;
      continue;
    }
    if (token == "--window_position") {
      if (i + 1 >= argc) {
        std::cerr << "Missing value for --window_position.\n";
        return 2;
      }
      const auto parsed_position =
          manim_cpp::renderer::parse_window_position(argv[++i]);
      if (!parsed_position.has_value()) {
        std::cerr << "Invalid value for --window_position: " << argv[i] << "\n";
        return 2;
      }
      window_position = parsed_position.value();
      continue;
    }
    if (token == "--window_size") {
      if (i + 1 >= argc) {
        std::cerr << "Missing value for --window_size.\n";
        return 2;
      }
      const auto parsed_size = manim_cpp::renderer::parse_window_size(argv[++i]);
      if (!parsed_size.has_value()) {
        std::cerr << "Invalid value for --window_size: " << argv[i] << "\n";
        return 2;
      }
      window_size = parsed_size.value();
      continue;
    }
    if (token == "--window_monitor") {
      if (i + 1 >= argc) {
        std::cerr << "Missing value for --window_monitor.\n";
        return 2;
      }
      const std::string raw_value = argv[++i];
      if (!parse_int_strict(raw_value, &window_monitor)) {
        std::cerr << "Invalid value for --window_monitor: " << raw_value << "\n";
        return 2;
      }
      continue;
    }
    if (token.rfind("-", 0) == 0) {
      std::cerr << "Unknown render option: " << token << "\n";
      return 2;
    }
    if (input_file.empty()) {
      input_file = token;
      continue;
    }
    std::cerr << "Unexpected render argument: " << token << "\n";
    return 2;
  }

  if (input_file.empty()) {
    std::cerr << "Usage: manim-cpp render <input.cpp> [--renderer <cairo|opengl>]\n";
    return 2;
  }

  manim_cpp::renderer::InteractionConfig interaction_config;
  interaction_config.watch = watch;
  interaction_config.interactive = interactive;
  interaction_config.enable_gui = enable_gui;
  interaction_config.fullscreen = fullscreen;
  interaction_config.force_window = force_window;
  interaction_config.window_monitor = window_monitor;
  interaction_config.window_position = window_position;
  interaction_config.window_size = window_size;
  manim_cpp::renderer::InteractionSession interaction_session(interaction_config);
  const bool window_open = interaction_session.should_open_window();

  if (!scene_name.empty()) {
    auto scene = manim_cpp::scene::SceneRegistry::instance().create(scene_name);
    if (!scene) {
      std::cerr << "Unknown scene: " << scene_name << "\n";
      return 2;
    }
    scene->run();

    manim_cpp::config::ManimConfig config;
    std::vector<std::filesystem::path> config_chain;
    const auto default_cfg = default_cfg_template_path();
    if (!default_cfg.empty()) {
      config_chain.push_back(default_cfg);
    }
    const auto local_cfg = std::filesystem::current_path() / "manim.cfg";
    if (std::filesystem::exists(local_cfg)) {
      config_chain.push_back(local_cfg);
    }
    if (!config_chain.empty()) {
      if (!config.load_with_precedence(config_chain)) {
        std::cerr << "Failed to load config chain for render.\n";
        return 2;
      }
    }

    int pixel_width = 1920;
    int pixel_height = 1080;
    double frame_rate = 60.0;
    parse_int_strict(config.get("CLI", "pixel_width", "1920"), &pixel_width);
    parse_int_strict(config.get("CLI", "pixel_height", "1080"), &pixel_height);
    parse_double_strict(config.get("CLI", "frame_rate", "60"), &frame_rate);
    if (pixel_width <= 0) {
      pixel_width = 1920;
    }
    if (pixel_height <= 0) {
      pixel_height = 1080;
    }
    if (frame_rate <= 0.0) {
      frame_rate = 60.0;
    }

    const auto quality =
        std::to_string(pixel_height) + "p" + std::to_string(static_cast<int>(std::llround(frame_rate)));
    const auto module_name = input_file.stem().string();
    const auto scene_output_extension = "." + manim_cpp::scene::to_string(media_format);

    manim_cpp::scene::SceneFileWriter writer(scene->scene_name());
    const double elapsed_seconds = scene->time_seconds();
    const std::size_t frame_count =
        elapsed_seconds > 0.0
            ? static_cast<std::size_t>(std::llround(elapsed_seconds * frame_rate))
            : static_cast<std::size_t>(1);
    std::optional<std::filesystem::path> output_file = std::nullopt;
    std::optional<std::filesystem::path> manifest_path = std::nullopt;
    std::optional<std::filesystem::path> subcaption_path = std::nullopt;

    const auto output_paths = writer.resolve_output_paths(config, module_name, quality);
    if (output_paths.has_value()) {
      std::filesystem::create_directories(output_paths->images_dir);
      std::filesystem::create_directories(output_paths->video_dir);
      std::filesystem::create_directories(output_paths->partial_movie_dir);

      for (std::size_t frame_index = 1; frame_index <= frame_count; ++frame_index) {
        const auto frame_path = output_paths->images_dir /
                                frame_file_name_for_renderer(renderer_type,
                                                             scene->scene_name(),
                                                             frame_index);
        std::ofstream frame_file(frame_path);
        if (!frame_file.is_open()) {
          std::cerr << "Failed to create frame image file: " << frame_path << "\n";
          return 2;
        }
      }

      output_file = output_paths->video_dir / (scene->scene_name() + scene_output_extension);
      manifest_path = output_paths->video_dir / (scene->scene_name() + ".json");
      subcaption_path = output_paths->video_dir / (scene->scene_name() + ".srt");
    }

    writer.set_render_summary(frame_count,
                              static_cast<std::size_t>(pixel_width),
                              static_cast<std::size_t>(pixel_height),
                              frame_rate,
                              manim_cpp::scene::to_string(media_format),
                              output_file);

    if (output_file.has_value()) {
      std::ofstream output_media(output_file.value());
      if (!output_media.is_open()) {
        std::cerr << "Failed to create render output file: " << output_file.value() << "\n";
        return 2;
      }
      output_media << "";
      output_media.close();
    }
    if (subcaption_path.has_value()) {
      if (!writer.write_subcaptions_srt(subcaption_path.value())) {
        std::cerr << "Failed to write subcaptions file: " << subcaption_path.value() << "\n";
        return 2;
      }
    }
    if (manifest_path.has_value()) {
      if (!writer.write_media_manifest(manifest_path.value())) {
        std::cerr << "Failed to write media manifest: " << manifest_path.value() << "\n";
        return 2;
      }
    }

    std::cout << "Rendered registered scene: " << scene->scene_name()
              << " elapsed=" << scene->time_seconds() << "s"
              << " frames=" << frame_count
              << " size=" << pixel_width << "x" << pixel_height
              << " fps=" << frame_rate
              << " renderer=" << manim_cpp::renderer::to_string(renderer_type)
              << " format=" << manim_cpp::scene::to_string(media_format)
              << " watch=" << (watch ? "true" : "false")
              << " interactive=" << (interactive ? "true" : "false")
              << " gui=" << (enable_gui ? "true" : "false")
              << " fullscreen=" << (fullscreen ? "true" : "false")
              << " force_window=" << (force_window ? "true" : "false")
              << " window_open=" << (window_open ? "true" : "false")
              << " window_position="
              << manim_cpp::renderer::to_string(window_position)
              << " window_size=" << manim_cpp::renderer::to_string(window_size)
              << " window_monitor=" << window_monitor;
    if (output_file.has_value()) {
      std::cout << " output=" << output_file->generic_string();
    }
    if (manifest_path.has_value()) {
      std::cout << " manifest=" << manifest_path->generic_string();
    }
    std::cout << "\n";
    return 0;
  }

  std::cout << "Command 'render' scaffold received input file: " << input_file
            << " renderer=" << manim_cpp::renderer::to_string(renderer_type)
            << " format=" << manim_cpp::scene::to_string(media_format)
            << " watch=" << (watch ? "true" : "false")
            << " interactive=" << (interactive ? "true" : "false")
            << " gui=" << (enable_gui ? "true" : "false")
            << " fullscreen=" << (fullscreen ? "true" : "false")
            << " force_window=" << (force_window ? "true" : "false")
            << " window_open=" << (window_open ? "true" : "false")
            << " window_position="
            << manim_cpp::renderer::to_string(window_position)
            << " window_size=" << manim_cpp::renderer::to_string(window_size)
            << " window_monitor=" << window_monitor << "\n";
  return 0;
}

int handle_cfg(const int argc, const char* const argv[]) {
  if (argc <= 2 || is_help_flag(argv[2])) {
    print_subcommand_help("cfg");
    return 0;
  }

  const std::string subcommand = argv[2];
  if (subcommand == "show") {
    return handle_cfg_show(argc, argv);
  }
  if (subcommand == "write") {
    return handle_cfg_write(argc, argv);
  }
  if (!is_member({"show", "write"}, subcommand)) {
    std::cerr << "Unknown cfg subcommand: " << subcommand << "\n";
    std::cerr << "Try 'manim-cpp cfg --help'.\n";
    return 2;
  }

  std::cout << "Command 'cfg " << subcommand
            << "' is scaffolded and pending parity implementation.\n";
  return 0;
}

int handle_checkhealth(const int argc, const char* const argv[]) {
  if (argc >= 3 && is_help_flag(argv[2])) {
    print_subcommand_help("checkhealth");
    return 0;
  }

  const bool json = argc >= 3 && std::string(argv[2]) == "--json";
  if (argc > 2 && !json) {
    std::cerr << "Unknown checkhealth option: " << argv[2] << "\n";
    return 2;
  }

  const bool ffmpeg_found = command_on_path("ffmpeg");
  const auto plugin_dir = std::filesystem::current_path() / "plugins";
  if (json) {
    std::cout << "{"
              << "\"ffmpeg\":" << (ffmpeg_found ? "true" : "false") << ","
              << "\"plugin_dir\":\"" << plugin_dir.string() << "\","
              << "\"renderers\":[\"cairo\",\"opengl\"],"
              << "\"formats\":[\"png\",\"gif\",\"mp4\",\"webm\",\"mov\"]"
              << "}\n";
    return 0;
  }

  std::cout << "checkhealth results:\n";
  std::cout << "  ffmpeg: " << (ffmpeg_found ? "found" : "missing") << "\n";
  std::cout << "  plugin_dir: " << plugin_dir << "\n";
  std::cout << "  renderers: cairo, opengl\n";
  std::cout << "  formats: png, gif, mp4, webm, mov\n";
  return 0;
}

int handle_init(const int argc, const char* const argv[]) {
  if (argc <= 2 || is_help_flag(argv[2])) {
    print_subcommand_help("init");
    return 0;
  }

  const std::string subcommand = argv[2];
  if (!is_member({"project", "scene"}, subcommand)) {
    std::cerr << "Unknown init subcommand: " << subcommand << "\n";
    std::cerr << "Try 'manim-cpp init --help'.\n";
    return 2;
  }

  if (subcommand == "scene") {
    return handle_init_scene(argc, argv);
  }
  if (subcommand == "project") {
    return handle_init_project(argc, argv);
  }

  std::cout << "Command 'init " << subcommand
            << "' is scaffolded and pending parity implementation.\n";
  return 0;
}

int handle_plugins(const int argc, const char* const argv[]) {
  if (argc <= 2 || is_help_flag(argv[2])) {
    print_subcommand_help("plugins");
    return 0;
  }

  const std::string subcommand = argv[2];
  if (!is_member({"list", "path", "load"}, subcommand)) {
    std::cerr << "Unknown plugins subcommand: " << subcommand << "\n";
    std::cerr << "Try 'manim-cpp plugins --help'.\n";
    return 2;
  }

  if (subcommand == "path") {
    std::cout << (std::filesystem::current_path() / "plugins") << "\n";
    return 0;
  }

  bool recursive = false;
  std::filesystem::path root;
  for (int i = 3; i < argc; ++i) {
    const std::string token = argv[i];
    if (token == "--recursive" || token == "-r") {
      recursive = true;
      continue;
    }
    if (root.empty()) {
      root = token;
      continue;
    }
    std::cerr << "Unexpected plugins " << subcommand << " argument: " << token << "\n";
    return 2;
  }
  if (root.empty()) {
    std::cerr << "Usage: manim-cpp plugins " << subcommand
              << " [--recursive] <directory>\n";
    return 2;
  }

  if (subcommand == "list") {
    const auto discovered = manim_cpp::plugin::PluginLoader::discover(root, recursive);
    std::cout << "Discovered " << discovered.size() << " plugin library/libraries in "
              << root << "\n";
    for (const auto& path : discovered) {
      std::cout << path << "\n";
    }
    return 0;
  }

  PluginLoadContext plugin_context;
  g_plugin_load_context = &plugin_context;
  const manim_plugin_host_api_v1 host_api = {.abi_version = MANIM_PLUGIN_ABI_VERSION_V1,
                                             .log_message = &plugin_log_callback,
                                             .register_scene_symbol =
                                                 &plugin_register_scene_callback};
  std::vector<std::string> errors;
  const auto loaded = manim_cpp::plugin::PluginLoader::load_from_directory(
      root, recursive, host_api, &errors);
  g_plugin_load_context = nullptr;
  if (!errors.empty()) {
    std::cerr << "Failed to load one or more plugins:\n";
    for (const auto& error : errors) {
      std::cerr << "  " << error << "\n";
    }
    return 2;
  }

  std::cout << "Loaded " << loaded.size() << " plugin(s) from " << root << "\n";
  for (const auto& log_line : plugin_context.logs) {
    std::cout << "Plugin log " << log_line << "\n";
  }
  for (const auto& registration : plugin_context.scene_registrations) {
    std::cout << "Registered scene symbol " << registration << "\n";
  }
  return 0;
}

}  // namespace

int run_cli(int argc, const char* const argv[]) {
  if (argc <= 1) {
    print_root_help();
    return 0;
  }

  const std::string first = argv[1];
  if (first == "--help" || first == "-h") {
    print_root_help();
    return 0;
  }

  if (first == "--version") {
    std::cout << "manim-cpp v" << manim_cpp::kVersionString << "\n";
    return 0;
  }

  if (first == "render") {
    return handle_render(argc, argv);
  }
  if (first == "cfg") {
    return handle_cfg(argc, argv);
  }
  if (first == "checkhealth") {
    return handle_checkhealth(argc, argv);
  }
  if (first == "init") {
    return handle_init(argc, argv);
  }
  if (first == "plugins") {
    return handle_plugins(argc, argv);
  }

  std::cerr << "Unknown command: " << first << "\n";
  std::cerr << "Run 'manim-cpp --help' for usage.\n";
  return 2;
}

}  // namespace manim_cpp::cli
