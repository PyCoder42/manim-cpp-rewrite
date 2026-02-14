#pragma once

#include <string>

namespace manim_cpp::migrate {

std::string translate_python_scene_to_cpp(const std::string& source_text,
                                          std::string* report);

int run_migrate(int argc, const char* const argv[]);

}  // namespace manim_cpp::migrate
