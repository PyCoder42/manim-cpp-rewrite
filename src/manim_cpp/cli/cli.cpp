#include "manim_cpp/cli/cli.hpp"

#include <iostream>
#include <string>

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

  if (first == "render" || first == "cfg" || first == "checkhealth" ||
      first == "init" || first == "plugins") {
    if (argc >= 3 &&
        (std::string(argv[2]) == "--help" || std::string(argv[2]) == "-h")) {
      print_subcommand_help(first);
      return 0;
    }

    std::cout << "Command '" << first
              << "' is scaffolded and pending parity implementation.\n";
    return 0;
  }

  std::cerr << "Unknown command: " << first << "\n";
  std::cerr << "Run 'manim-cpp --help' for usage.\n";
  return 2;
}

}  // namespace manim_cpp::cli
