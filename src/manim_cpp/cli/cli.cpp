#include "manim_cpp/cli/cli.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

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
  if (argc > 2) {
    std::cerr << "checkhealth does not accept positional arguments yet.\n";
    return 2;
  }

  std::cout << "Command 'checkhealth' is scaffolded and pending parity implementation.\n";
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

  std::cout << "Command 'plugins " << subcommand
            << "' is scaffolded and pending parity implementation.\n";
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
