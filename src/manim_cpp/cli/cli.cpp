#include "manim_cpp/cli/cli.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "manim_cpp/config/config.hpp"
#include "manim_cpp/plugin/loader.hpp"
#include "manim_cpp/renderer/renderer.hpp"
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
  std::cout << "Usage: manim-cpp " << command << " [OPTIONS]\n";
}

bool is_help_flag(const std::string& token) {
  return token == "--help" || token == "-h";
}

bool is_member(const std::vector<std::string>& values, const std::string& target) {
  return std::find(values.begin(), values.end(), target) != values.end();
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

  std::cout << "Command 'render' scaffold received input file: " << input_file
            << " renderer=" << manim_cpp::renderer::to_string(renderer_type) << "\n";
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
              << "\"plugin_dir\":\"" << plugin_dir.string() << "\""
              << "}\n";
    return 0;
  }

  std::cout << "checkhealth results:\n";
  std::cout << "  ffmpeg: " << (ffmpeg_found ? "found" : "missing") << "\n";
  std::cout << "  plugin_dir: " << plugin_dir << "\n";
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
  if (!is_member({"list", "path"}, subcommand)) {
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
    std::cerr << "Unexpected plugins list argument: " << token << "\n";
    return 2;
  }
  if (root.empty()) {
    std::cerr << "Usage: manim-cpp plugins list [--recursive] <directory>\n";
    return 2;
  }

  const auto discovered = manim_cpp::plugin::PluginLoader::discover(root, recursive);
  std::cout << "Discovered " << discovered.size() << " plugin library/libraries in "
            << root << "\n";
  for (const auto& path : discovered) {
    std::cout << path << "\n";
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
