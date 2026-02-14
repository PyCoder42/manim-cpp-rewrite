#include "manim_cpp/migrate/migrate.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace manim_cpp::migrate {
namespace {

struct MigrateArgs {
  std::filesystem::path input_path;
  std::filesystem::path output_path;
  bool write_output = false;
};

std::string trim(const std::string& text) {
  const auto first = text.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) {
    return "";
  }
  const auto last = text.find_last_not_of(" \t\r\n");
  return text.substr(first, last - first + 1);
}

bool parse_args(int argc, const char* const argv[], MigrateArgs* out_args) {
  if (argc < 2) {
    return false;
  }

  out_args->input_path = argv[1];
  for (int i = 2; i < argc; ++i) {
    const std::string token = argv[i];
    if (token == "--out" && i + 1 < argc) {
      out_args->output_path = argv[++i];
      out_args->write_output = true;
      continue;
    }
  }
  return true;
}

std::vector<std::string> detect_scene_classes(const std::string& source_text) {
  std::vector<std::string> classes;
  std::regex scene_class_pattern(
      R"(^\s*class\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(\s*Scene\s*\)\s*:)",
      std::regex_constants::ECMAScript | std::regex_constants::multiline);

  for (std::sregex_iterator it(source_text.begin(), source_text.end(),
                               scene_class_pattern);
       it != std::sregex_iterator(); ++it) {
    classes.push_back((*it)[1].str());
  }
  return classes;
}

std::vector<std::string> detect_dynamic_hazards(const std::string& source_text) {
  static const std::vector<std::string> kHazardTokens = {
      "lambda", "exec(", "eval(", "setattr(", "getattr(", "__getattr__"};
  std::vector<std::string> hazards;
  for (const auto& token : kHazardTokens) {
    if (source_text.find(token) != std::string::npos) {
      hazards.push_back(token);
    }
  }
  return hazards;
}

}  // namespace

std::string translate_python_scene_to_cpp(const std::string& source_text,
                                          std::string* report) {
  const auto scene_classes = detect_scene_classes(source_text);
  const auto hazards = detect_dynamic_hazards(source_text);

  std::ostringstream converted;
  converted << "#include \"manim_cpp/scene/scene.hpp\"\n";
  converted << "#include \"manim_cpp/scene/registry.hpp\"\n\n";
  converted << "using namespace manim_cpp::scene;\n\n";

  if (scene_classes.empty()) {
    converted << "// TODO(migrate): No Scene subclasses detected.\n";
  }

  for (const auto& class_name : scene_classes) {
    converted << "class " << class_name << " : public Scene {\n";
    converted << " public:\n";
    converted << "  std::string scene_name() const override { return \""
              << class_name << "\"; }\n";
    converted << "  void construct() override {\n";
    converted
        << "    // TODO(migrate): port Python construct() body manually.\n";
    converted << "  }\n";
    converted << "};\n\n";
    converted << "MANIM_REGISTER_SCENE(" << class_name << ");\n\n";
  }

  if (!hazards.empty()) {
    converted << "// TODO(migrate): dynamic Python patterns detected: ";
    for (size_t i = 0; i < hazards.size(); ++i) {
      converted << hazards[i];
      if (i + 1 < hazards.size()) {
        converted << ", ";
      }
    }
    converted << "\n";
  }

  if (report != nullptr) {
    std::ostringstream summary;
    summary << "scenes_detected=" << scene_classes.size()
            << " hazards_detected=" << hazards.size();
    *report = summary.str();
  }

  return converted.str();
}

int run_migrate(int argc, const char* const argv[]) {
  if (argc <= 1 || std::string(argv[1]) == "--help" ||
      std::string(argv[1]) == "-h") {
    std::cout
        << "Usage: manim-cpp-migrate <python_scene.py> [--out <file.cpp>]\n";
    std::cout << "Deterministic Python-to-C++ migration scaffolder.\n";
    return 0;
  }

  MigrateArgs args;
  if (!parse_args(argc, argv, &args)) {
    std::cerr << "Invalid arguments.\n";
    return 2;
  }

  std::ifstream input(args.input_path);
  if (!input.is_open()) {
    std::cerr << "Unable to open input file: " << args.input_path << "\n";
    return 2;
  }
  std::stringstream source_buffer;
  source_buffer << input.rdbuf();

  std::string report;
  const std::string output = translate_python_scene_to_cpp(source_buffer.str(), &report);

  if (args.write_output) {
    std::ofstream out(args.output_path);
    if (!out.is_open()) {
      std::cerr << "Unable to write output file: " << args.output_path << "\n";
      return 2;
    }
    out << output;
    std::cout << "Wrote translated C++ scaffold to " << args.output_path << "\n";
  } else {
    std::cout << output;
  }

  std::cout << "Migration report: " << report << "\n";
  return 0;
}

}  // namespace manim_cpp::migrate
