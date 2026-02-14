#include "manim_cpp/migrate/migrate.hpp"

#include <algorithm>
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
  std::filesystem::path output_dir;
  std::filesystem::path report_path;
  bool write_output = false;
  bool write_output_dir = false;
  bool write_report = false;
  bool recursive = false;
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
    if (token == "--out-dir" && i + 1 < argc) {
      out_args->output_dir = argv[++i];
      out_args->write_output_dir = true;
      continue;
    }
    if (token == "--report" && i + 1 < argc) {
      out_args->report_path = argv[++i];
      out_args->write_report = true;
      continue;
    }
    if (token == "--recursive") {
      out_args->recursive = true;
      continue;
    }
  }
  return true;
}

std::vector<std::string> detect_scene_classes(const std::string& source_text) {
  std::vector<std::string> classes;
  std::regex scene_class_pattern(R"(^\s*class\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(([^)]*)\)\s*:)",
                                 std::regex_constants::ECMAScript |
                                     std::regex_constants::multiline);
  const std::vector<std::string> kSupportedBases = {
      "Scene", "ThreeDScene", "MovingCameraScene"};

  for (std::sregex_iterator it(source_text.begin(), source_text.end(),
                               scene_class_pattern);
       it != std::sregex_iterator(); ++it) {
    const std::string class_name = (*it)[1].str();
    const std::string bases = (*it)[2].str();

    std::stringstream base_stream(bases);
    std::string base;
    while (std::getline(base_stream, base, ',')) {
      if (std::find(kSupportedBases.begin(), kSupportedBases.end(), trim(base)) ==
          kSupportedBases.end()) {
        continue;
      }
      classes.push_back(class_name);
      break;
    }
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

std::vector<std::string> detect_construct_calls(const std::string& source_text) {
  std::vector<std::string> calls;
  std::regex call_pattern(R"(^\s*self\.[A-Za-z_][A-Za-z0-9_]*\s*\(.*\)\s*$)",
                          std::regex_constants::ECMAScript |
                              std::regex_constants::multiline);

  for (std::sregex_iterator it(source_text.begin(), source_text.end(),
                               call_pattern);
       it != std::sregex_iterator(); ++it) {
    calls.push_back(trim(it->str()));
  }
  return calls;
}

bool read_text_file(const std::filesystem::path& input_path, std::string* output) {
  std::ifstream input(input_path);
  if (!input.is_open()) {
    return false;
  }
  std::stringstream buffer;
  buffer << input.rdbuf();
  *output = buffer.str();
  return true;
}

bool write_text_file(const std::filesystem::path& output_path,
                     const std::string& text) {
  if (output_path.has_parent_path()) {
    std::filesystem::create_directories(output_path.parent_path());
  }
  std::ofstream out(output_path);
  if (!out.is_open()) {
    return false;
  }
  out << text;
  return true;
}

std::vector<std::filesystem::path> discover_python_files(
    const std::filesystem::path& input_dir,
    const bool recursive) {
  std::vector<std::filesystem::path> files;
  if (recursive) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(input_dir)) {
      if (!entry.is_regular_file() || entry.path().extension() != ".py") {
        continue;
      }
      files.push_back(entry.path());
    }
  } else {
    for (const auto& entry : std::filesystem::directory_iterator(input_dir)) {
      if (!entry.is_regular_file() || entry.path().extension() != ".py") {
        continue;
      }
      files.push_back(entry.path());
    }
  }
  std::sort(files.begin(), files.end());
  return files;
}

}  // namespace

std::string translate_python_scene_to_cpp(const std::string& source_text,
                                          std::string* report) {
  const auto scene_classes = detect_scene_classes(source_text);
  const auto hazards = detect_dynamic_hazards(source_text);
  const auto calls = detect_construct_calls(source_text);

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
    for (const auto& call : calls) {
      converted << "    // TODO(migrate): original call -> " << call << "\n";
    }
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
            << " hazards_detected=" << hazards.size()
            << " calls_detected=" << calls.size();
    *report = summary.str();
  }

  return converted.str();
}

int run_migrate(int argc, const char* const argv[]) {
  if (argc <= 1 || std::string(argv[1]) == "--help" ||
      std::string(argv[1]) == "-h") {
    std::cout << "Usage: manim-cpp-migrate <python_scene.py|directory> "
                 "[--out <file.cpp>] [--out-dir <directory>] [--recursive]\n";
    std::cout << "Deterministic Python-to-C++ migration scaffolder.\n";
    return 0;
  }

  MigrateArgs args;
  if (!parse_args(argc, argv, &args)) {
    std::cerr << "Invalid arguments.\n";
    return 2;
  }

  if (std::filesystem::is_directory(args.input_path)) {
    if (!args.write_output_dir) {
      std::cerr << "Directory migration requires --out-dir <directory>.\n";
      return 2;
    }

    const auto python_files = discover_python_files(args.input_path, args.recursive);
    std::vector<std::string> report_lines;
    report_lines.reserve(python_files.size());

    for (const auto& input_path : python_files) {
      std::string source_text;
      if (!read_text_file(input_path, &source_text)) {
        std::cerr << "Unable to open input file: " << input_path << "\n";
        return 2;
      }

      std::string report;
      const std::string output = translate_python_scene_to_cpp(source_text, &report);
      const auto relative_path = std::filesystem::relative(input_path, args.input_path);
      auto output_path = args.output_dir / relative_path;
      output_path.replace_extension(".cpp");
      if (!write_text_file(output_path, output)) {
        std::cerr << "Unable to write output file: " << output_path << "\n";
        return 2;
      }
      report_lines.push_back(relative_path.string() + ": " + report);
      std::cout << "Wrote translated C++ scaffold to " << output_path << "\n";
    }

    if (args.write_report) {
      std::ostringstream report_text;
      for (const auto& line : report_lines) {
        report_text << line << "\n";
      }
      if (!write_text_file(args.report_path, report_text.str())) {
        std::cerr << "Unable to write report file: " << args.report_path << "\n";
        return 2;
      }
    }

    std::cout << "Migration report: files_processed=" << python_files.size() << "\n";
    return 0;
  }

  std::string source_text;
  if (!read_text_file(args.input_path, &source_text)) {
    std::cerr << "Unable to open input file: " << args.input_path << "\n";
    return 2;
  }

  std::string report;
  const std::string output = translate_python_scene_to_cpp(source_text, &report);

  std::filesystem::path output_path = args.output_path;
  bool write_output = args.write_output;
  if (args.write_output_dir) {
    output_path = args.output_dir / (args.input_path.stem().string() + ".cpp");
    write_output = true;
  }

  if (write_output) {
    if (!write_text_file(output_path, output)) {
      std::cerr << "Unable to write output file: " << output_path << "\n";
      return 2;
    }
    std::cout << "Wrote translated C++ scaffold to " << output_path << "\n";
  } else {
    std::cout << output;
  }

  if (args.write_report) {
    if (!write_text_file(args.report_path, report + "\n")) {
      std::cerr << "Unable to write report file: " << args.report_path << "\n";
      return 2;
    }
  }

  std::cout << "Migration report: " << report << "\n";
  return 0;
}

}  // namespace manim_cpp::migrate
