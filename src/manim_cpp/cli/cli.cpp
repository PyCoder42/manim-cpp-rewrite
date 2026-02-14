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

  std::cout << "Command 'render' scaffold received input file: " << argv[2] << "\n";
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

  if (argc < 4) {
    std::cerr << "Usage: manim-cpp plugins list <directory>\n";
    return 2;
  }

  const std::filesystem::path root = argv[3];
  const auto discovered = manim_cpp::plugin::PluginLoader::discover(root, false);
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
