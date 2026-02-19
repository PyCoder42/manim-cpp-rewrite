#include "manim_cpp/migrate/migrate.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace manim_cpp::migrate {
namespace {

struct SceneClass {
  std::string name;
  std::string base_class;
  std::size_t start_offset = 0;
  std::size_t end_offset = 0;
};

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

std::vector<SceneClass> detect_scene_classes(const std::string& source_text) {
  std::vector<SceneClass> classes;
  std::regex scene_class_pattern(R"(^\s*class\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(([^)]*)\)\s*:)",
                                 std::regex_constants::ECMAScript |
                                     std::regex_constants::multiline);
  const std::vector<std::string> kSupportedBases = {
      "Scene", "ThreeDScene", "MovingCameraScene", "ZoomedScene"};

  for (std::sregex_iterator it(source_text.begin(), source_text.end(),
                               scene_class_pattern);
       it != std::sregex_iterator(); ++it) {
    const std::string class_name = (*it)[1].str();
    const std::string bases = (*it)[2].str();
    const auto class_start = static_cast<std::size_t>(it->position());

    std::stringstream base_stream(bases);
    std::string base;
    while (std::getline(base_stream, base, ',')) {
      const std::string trimmed_base = trim(base);
      if (std::find(kSupportedBases.begin(), kSupportedBases.end(), trimmed_base) ==
          kSupportedBases.end()) {
        continue;
      }
      classes.push_back(SceneClass{
          .name = class_name,
          .base_class = trimmed_base,
          .start_offset = class_start,
          .end_offset = source_text.size(),
      });
      break;
    }
  }

  for (std::size_t i = 0; i + 1 < classes.size(); ++i) {
    classes[i].end_offset = classes[i + 1].start_offset;
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

std::string scene_source_block(const std::string& source_text,
                               const SceneClass& scene_class) {
  if (scene_class.start_offset >= source_text.size() ||
      scene_class.end_offset <= scene_class.start_offset) {
    return {};
  }
  const auto length = scene_class.end_offset - scene_class.start_offset;
  return source_text.substr(scene_class.start_offset, length);
}

bool is_numeric_literal(const std::string& text) {
  static const std::regex kPattern(
      R"(^[+-]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)$)",
      std::regex_constants::ECMAScript);
  return std::regex_match(text, kPattern);
}

bool is_integer_literal(const std::string& text) {
  static const std::regex kPattern(R"(^[+-]?[0-9]+$)",
                                   std::regex_constants::ECMAScript);
  return std::regex_match(text, kPattern);
}

bool is_identifier(const std::string& text) {
  static const std::regex kPattern(R"(^[A-Za-z_][A-Za-z0-9_]*$)",
                                   std::regex_constants::ECMAScript);
  return std::regex_match(text, kPattern);
}

std::optional<std::string> parse_wait_argument(const std::string& argument) {
  if (argument.empty()) {
    return std::string("1.0");
  }
  if (is_numeric_literal(argument)) {
    return argument;
  }

  static const std::regex kDurationKeywordPattern(
      R"(^duration\s*=\s*([+-]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+))$)",
      std::regex_constants::ECMAScript);
  std::smatch keyword_match;
  if (std::regex_match(argument, keyword_match, kDurationKeywordPattern)) {
    return keyword_match[1].str();
  }
  return std::nullopt;
}

bool is_supported_geometry_constructor(const std::string& class_name) {
  static const std::vector<std::string> kSupportedConstructors = {
      "Dot",    "Circle",         "Square",    "Rectangle",
      "Triangle", "RegularPolygon", "Ellipse",   "Arc",
      "Line",
  };
  return std::find(kSupportedConstructors.begin(), kSupportedConstructors.end(),
                   class_name) != kSupportedConstructors.end();
}

std::vector<std::string> split_top_level_arguments(const std::string& text,
                                                   bool* ok) {
  if (ok != nullptr) {
    *ok = true;
  }

  const std::string trimmed = trim(text);
  if (trimmed.empty()) {
    return {};
  }

  std::vector<std::string> args;
  std::string current;
  int paren_depth = 0;
  int bracket_depth = 0;
  int brace_depth = 0;
  char quote = '\0';
  bool escaped = false;

  for (const char ch : trimmed) {
    if (quote != '\0') {
      current.push_back(ch);
      if (escaped) {
        escaped = false;
      } else if (ch == '\\') {
        escaped = true;
      } else if (ch == quote) {
        quote = '\0';
      }
      continue;
    }

    if (ch == '\'' || ch == '"') {
      quote = ch;
      current.push_back(ch);
      continue;
    }

    if (ch == '(') {
      ++paren_depth;
      current.push_back(ch);
      continue;
    }
    if (ch == ')') {
      --paren_depth;
      current.push_back(ch);
      if (paren_depth < 0) {
        if (ok != nullptr) {
          *ok = false;
        }
        return {};
      }
      continue;
    }
    if (ch == '[') {
      ++bracket_depth;
      current.push_back(ch);
      continue;
    }
    if (ch == ']') {
      --bracket_depth;
      current.push_back(ch);
      if (bracket_depth < 0) {
        if (ok != nullptr) {
          *ok = false;
        }
        return {};
      }
      continue;
    }
    if (ch == '{') {
      ++brace_depth;
      current.push_back(ch);
      continue;
    }
    if (ch == '}') {
      --brace_depth;
      current.push_back(ch);
      if (brace_depth < 0) {
        if (ok != nullptr) {
          *ok = false;
        }
        return {};
      }
      continue;
    }

    if (ch == ',' && paren_depth == 0 && bracket_depth == 0 && brace_depth == 0) {
      const std::string token = trim(current);
      if (token.empty()) {
        if (ok != nullptr) {
          *ok = false;
        }
        return {};
      }
      args.push_back(token);
      current.clear();
      continue;
    }

    current.push_back(ch);
  }

  if (quote != '\0' || paren_depth != 0 || bracket_depth != 0 || brace_depth != 0) {
    if (ok != nullptr) {
      *ok = false;
    }
    return {};
  }

  const std::string token = trim(current);
  if (token.empty()) {
    if (ok != nullptr) {
      *ok = false;
    }
    return {};
  }
  args.push_back(token);
  return args;
}

bool contains_top_level_assignment(const std::string& text) {
  int paren_depth = 0;
  int bracket_depth = 0;
  int brace_depth = 0;
  char quote = '\0';
  bool escaped = false;

  for (const char ch : text) {
    if (quote != '\0') {
      if (escaped) {
        escaped = false;
      } else if (ch == '\\') {
        escaped = true;
      } else if (ch == quote) {
        quote = '\0';
      }
      continue;
    }

    if (ch == '\'' || ch == '"') {
      quote = ch;
      continue;
    }
    if (ch == '(') {
      ++paren_depth;
      continue;
    }
    if (ch == ')') {
      --paren_depth;
      continue;
    }
    if (ch == '[') {
      ++bracket_depth;
      continue;
    }
    if (ch == ']') {
      --bracket_depth;
      continue;
    }
    if (ch == '{') {
      ++brace_depth;
      continue;
    }
    if (ch == '}') {
      --brace_depth;
      continue;
    }

    if (ch == '=' && paren_depth == 0 && bracket_depth == 0 && brace_depth == 0) {
      return true;
    }
  }
  return false;
}

struct ConstructTranslationContext {
  std::vector<std::string> declaration_lines;
  std::unordered_map<std::string, std::string> known_mobject_variables;
  std::size_t generated_symbol_index = 0;
};

std::optional<std::string> translate_geometry_constructor_expression(
    const std::string& expression) {
  static const std::regex kCtorPattern(
      R"(^([A-Za-z_][A-Za-z0-9_]*)\((.*)\)$)",
      std::regex_constants::ECMAScript);
  std::smatch match;
  if (!std::regex_match(expression, match, kCtorPattern)) {
    return std::nullopt;
  }

  const std::string class_name = trim(match[1].str());
  if (!is_supported_geometry_constructor(class_name)) {
    return std::nullopt;
  }

  const std::string ctor_args = trim(match[2].str());
  if (contains_top_level_assignment(ctor_args)) {
    return std::nullopt;
  }
  if (ctor_args.empty()) {
    return "std::make_shared<manim_cpp::mobject::" + class_name + ">()";
  }
  return "std::make_shared<manim_cpp::mobject::" + class_name + ">(" + ctor_args +
         ")";
}

std::string next_generated_symbol(const std::string& prefix,
                                  ConstructTranslationContext* context) {
  context->generated_symbol_index += 1;
  return "migrate_" + prefix + "_" + std::to_string(context->generated_symbol_index);
}

std::optional<std::string> resolve_mobject_expression_for_animation(
    const std::string& expression, ConstructTranslationContext* context,
    std::vector<std::string>* preamble_lines) {
  if (is_identifier(expression) &&
      context->known_mobject_variables.find(expression) !=
          context->known_mobject_variables.end()) {
    return expression;
  }

  const auto converted = translate_geometry_constructor_expression(expression);
  if (!converted.has_value()) {
    return std::nullopt;
  }

  const std::string symbol = next_generated_symbol("obj", context);
  preamble_lines->push_back("auto " + symbol + " = " + converted.value() + ";");
  return symbol;
}

std::optional<std::string> translate_add_or_remove_call(const std::string& method,
                                                        const std::string& argument_text,
                                                        const ConstructTranslationContext& context) {
  bool parsed_ok = false;
  const auto arguments = split_top_level_arguments(argument_text, &parsed_ok);
  if (!parsed_ok || arguments.empty()) {
    return std::nullopt;
  }

  std::ostringstream translated;
  for (std::size_t i = 0; i < arguments.size(); ++i) {
    const std::string argument = trim(arguments[i]);
    std::string translated_argument;
    if (is_identifier(argument) &&
        context.known_mobject_variables.find(argument) !=
            context.known_mobject_variables.end()) {
      translated_argument = argument;
    } else {
      const auto converted = translate_geometry_constructor_expression(argument);
      if (!converted.has_value()) {
        return std::nullopt;
      }
      translated_argument = converted.value();
    }
    if (i > 0) {
      translated << "\n    ";
    }
    translated << method << "(" << translated_argument << ");";
  }
  return translated.str();
}

std::optional<std::string> translate_play_call(const std::string& argument_text,
                                               ConstructTranslationContext* context) {
  bool parsed_ok = false;
  const auto arguments = split_top_level_arguments(argument_text, &parsed_ok);
  if (!parsed_ok || arguments.empty()) {
    return std::nullopt;
  }

  std::optional<std::string> run_time_literal;
  static const std::regex kRunTimePattern(
      R"(^run_time\s*=\s*([+-]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+))$)",
      std::regex_constants::ECMAScript);

  std::size_t animation_arg_count = arguments.size();
  {
    std::smatch run_time_match;
    const std::string run_time_token = trim(arguments.back());
    if (std::regex_match(run_time_token, run_time_match, kRunTimePattern)) {
      run_time_literal = run_time_match[1].str();
      animation_arg_count -= 1;
    }
  }
  if (animation_arg_count == 0) {
    return std::nullopt;
  }
  if (run_time_literal.has_value() && animation_arg_count > 1) {
    return std::nullopt;
  }

  static const std::regex kAnimationPattern(
      R"(^\s*(FadeIn|FadeOut|Create|Write)\((.*)\)\s*$)",
      std::regex_constants::ECMAScript);
  std::vector<std::string> translated_lines;

  for (std::size_t animation_index = 0; animation_index < animation_arg_count;
       ++animation_index) {
    std::smatch match;
    const std::string animation_argument = trim(arguments[animation_index]);
    if (!std::regex_match(animation_argument, match, kAnimationPattern)) {
      return std::nullopt;
    }

    const std::string animation_name = trim(match[1].str());
    const std::string target_expression = trim(match[2].str());
    std::vector<std::string> lines;

    if (animation_name == "Create" || animation_name == "Write") {
      std::string add_line;
      if (is_identifier(target_expression) &&
          context->known_mobject_variables.find(target_expression) !=
              context->known_mobject_variables.end()) {
        add_line = "add(" + target_expression + ");";
      } else {
        const auto converted_target =
            translate_geometry_constructor_expression(target_expression);
        if (!converted_target.has_value()) {
          return std::nullopt;
        }
        add_line = "add(" + converted_target.value() + ");";
      }
      lines.push_back(add_line);
      if (run_time_literal.has_value()) {
        lines.push_back("wait(" + run_time_literal.value() + ");");
      }
      translated_lines.insert(translated_lines.end(), lines.begin(), lines.end());
      continue;
    }

    const auto target_symbol = resolve_mobject_expression_for_animation(
        target_expression, context, &lines);
    if (!target_symbol.has_value()) {
      return std::nullopt;
    }

    if (animation_name == "FadeIn") {
      lines.push_back(target_symbol.value() + "->set_opacity(0.0);");
      lines.push_back("add(" + target_symbol.value() + ");");
      const std::string fade_symbol = next_generated_symbol("fade_in", context);
      lines.push_back("manim_cpp::animation::FadeToOpacityAnimation " + fade_symbol +
                      "(" + target_symbol.value() + ", 1.0);");
      if (run_time_literal.has_value()) {
        lines.push_back(fade_symbol + ".set_run_time_seconds(" +
                        run_time_literal.value() + ");");
      }
      lines.push_back("play(" + fade_symbol + ");");
    } else if (animation_name == "FadeOut") {
      const std::string fade_symbol = next_generated_symbol("fade_out", context);
      lines.push_back("manim_cpp::animation::FadeToOpacityAnimation " + fade_symbol +
                      "(" + target_symbol.value() + ", 0.0);");
      if (run_time_literal.has_value()) {
        lines.push_back(fade_symbol + ".set_run_time_seconds(" +
                        run_time_literal.value() + ");");
      }
      lines.push_back("play(" + fade_symbol + ");");
      lines.push_back("remove(" + target_symbol.value() + ");");
    } else {
      return std::nullopt;
    }
    translated_lines.insert(translated_lines.end(), lines.begin(), lines.end());
  }

  std::ostringstream translated;
  for (std::size_t i = 0; i < translated_lines.size(); ++i) {
    if (i > 0) {
      translated << "\n    ";
    }
    translated << translated_lines[i];
  }
  return translated.str();
}

std::optional<std::string> translate_construct_call(
    const std::string& call, ConstructTranslationContext* context) {
  static const std::regex kWaitPattern(
      R"(^self\.wait\(([^)]*)\)$)",
      std::regex_constants::ECMAScript);
  static const std::regex kClearPattern(
      R"(^self\.clear\(\)$)",
      std::regex_constants::ECMAScript);
  static const std::regex kAddPattern(
      R"(^self\.add\((.*)\)$)",
      std::regex_constants::ECMAScript);
  static const std::regex kRemovePattern(
      R"(^self\.remove\((.*)\)$)",
      std::regex_constants::ECMAScript);
  static const std::regex kSetRandomSeedPattern(
      R"(^self\.set_random_seed\(([^)]*)\)$)",
      std::regex_constants::ECMAScript);
  static const std::regex kClearUpdatersPattern(
      R"(^self\.clear_updaters\(\)$)",
      std::regex_constants::ECMAScript);
  static const std::regex kPlayPattern(
      R"(^self\.play\((.*)\)$)",
      std::regex_constants::ECMAScript);

  std::smatch match;
  if (std::regex_match(call, match, kWaitPattern)) {
    const std::string argument = trim(match[1].str());
    const auto parsed_wait_argument = parse_wait_argument(argument);
    if (parsed_wait_argument.has_value()) {
      return "wait(" + parsed_wait_argument.value() + ");";
    }
    return std::nullopt;
  }

  if (std::regex_match(call, kClearPattern)) {
    return "clear();";
  }

  if (std::regex_match(call, match, kAddPattern)) {
    return translate_add_or_remove_call("add", trim(match[1].str()), *context);
  }

  if (std::regex_match(call, match, kRemovePattern)) {
    return translate_add_or_remove_call("remove", trim(match[1].str()), *context);
  }

  if (std::regex_match(call, match, kPlayPattern)) {
    return translate_play_call(trim(match[1].str()), context);
  }

  if (std::regex_match(call, match, kSetRandomSeedPattern)) {
    const std::string argument = trim(match[1].str());
    if (is_integer_literal(argument)) {
      return "set_random_seed(" + argument + ");";
    }
    return std::nullopt;
  }

  if (std::regex_match(call, kClearUpdatersPattern)) {
    return "clear_updaters();";
  }

  return std::nullopt;
}

ConstructTranslationContext detect_construct_context(const std::string& scene_block) {
  ConstructTranslationContext context;
  std::regex assignment_pattern(
      R"(^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*([A-Za-z_][A-Za-z0-9_]*\s*\(.*\))\s*$)",
      std::regex_constants::ECMAScript | std::regex_constants::multiline);

  for (std::sregex_iterator it(scene_block.begin(), scene_block.end(),
                               assignment_pattern);
       it != std::sregex_iterator(); ++it) {
    const std::string variable = trim((*it)[1].str());
    const std::string expression = trim((*it)[2].str());
    if (!is_identifier(variable)) {
      continue;
    }
    if (context.known_mobject_variables.find(variable) !=
        context.known_mobject_variables.end()) {
      continue;
    }
    const auto translated =
        translate_geometry_constructor_expression(expression);
    if (!translated.has_value()) {
      continue;
    }

    context.known_mobject_variables.emplace(variable, translated.value());
    context.declaration_lines.push_back("auto " + variable + " = " +
                                        translated.value() + ";");
  }

  return context;
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
  const bool uses_moving_camera_scene =
      std::any_of(scene_classes.begin(), scene_classes.end(), [](const SceneClass& scene) {
        return scene.base_class == "MovingCameraScene";
      });
  const bool uses_three_d_scene =
      std::any_of(scene_classes.begin(), scene_classes.end(), [](const SceneClass& scene) {
        return scene.base_class == "ThreeDScene";
      });
  const bool uses_zoomed_scene =
      std::any_of(scene_classes.begin(), scene_classes.end(), [](const SceneClass& scene) {
        return scene.base_class == "ZoomedScene";
      });
  std::size_t calls_detected = 0;
  std::size_t translated_calls = 0;

  std::ostringstream converted;
  converted << "#include <memory>\n";
  converted << "#include \"manim_cpp/animation/basic_animations.hpp\"\n";
  converted << "#include \"manim_cpp/mobject/geometry.hpp\"\n";
  converted << "#include \"manim_cpp/scene/scene.hpp\"\n";
  if (uses_moving_camera_scene) {
    converted << "#include \"manim_cpp/scene/moving_camera_scene.hpp\"\n";
  }
  if (uses_three_d_scene) {
    converted << "#include \"manim_cpp/scene/three_d_scene.hpp\"\n";
  }
  if (uses_zoomed_scene) {
    converted << "#include \"manim_cpp/scene/zoomed_scene.hpp\"\n";
  }
  converted << "#include \"manim_cpp/scene/registry.hpp\"\n\n";
  converted << "using namespace manim_cpp::scene;\n\n";

  if (scene_classes.empty()) {
    converted << "// TODO(migrate): No Scene subclasses detected.\n";
  }

  for (const auto& scene_class : scene_classes) {
    const auto class_source = scene_source_block(source_text, scene_class);
    const auto class_calls = detect_construct_calls(class_source);
    calls_detected += class_calls.size();
    auto context = detect_construct_context(class_source);

    converted << "class " << scene_class.name << " : public "
              << scene_class.base_class << " {\n";
    converted << " public:\n";
    converted << "  std::string scene_name() const override { return \""
              << scene_class.name << "\"; }\n";
    converted << "  void construct() override {\n";
    converted
        << "    // TODO(migrate): port Python construct() body manually.\n";
    for (const auto& declaration : context.declaration_lines) {
      converted << "    " << declaration << "\n";
    }
    for (const auto& call : class_calls) {
      const auto translated = translate_construct_call(call, &context);
      if (translated.has_value()) {
        converted << "    " << translated.value() << "\n";
        ++translated_calls;
      } else {
        converted << "    // TODO(migrate): original call -> " << call << "\n";
      }
    }
    converted << "  }\n";
    converted << "};\n\n";
    converted << "MANIM_REGISTER_SCENE(" << scene_class.name << ");\n\n";
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
            << " calls_detected=" << calls_detected
            << " translated_calls=" << translated_calls;
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
    if (python_files.empty()) {
      std::cerr << "No Python files found in input directory: " << args.input_path
                << "\n";
      return 2;
    }
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
