#include "manim_cpp/config/config.hpp"

#include <fstream>
#include <string>

namespace manim_cpp::config {
namespace {

std::string trim(const std::string& value) {
  const auto first = value.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) {
    return "";
  }
  const auto last = value.find_last_not_of(" \t\r\n");
  return value.substr(first, last - first + 1);
}

}  // namespace

bool ManimConfig::load_from_file(const std::filesystem::path& config_path) {
  std::ifstream file(config_path);
  if (!file.is_open()) {
    return false;
  }

  std::string active_section;
  std::string line;
  while (std::getline(file, line)) {
    const std::string cleaned = trim(line);
    if (cleaned.empty()) {
      continue;
    }
    if (cleaned[0] == '#' || cleaned[0] == ';') {
      continue;
    }

    if (cleaned.front() == '[' && cleaned.back() == ']') {
      active_section = trim(cleaned.substr(1, cleaned.size() - 2));
      data_[active_section];
      continue;
    }

    const auto equals_pos = cleaned.find('=');
    if (equals_pos == std::string::npos || active_section.empty()) {
      continue;
    }

    const std::string key = trim(cleaned.substr(0, equals_pos));
    const std::string value = trim(cleaned.substr(equals_pos + 1));
    if (!key.empty()) {
      data_[active_section][key] = value;
    }
  }

  return true;
}

bool ManimConfig::has(const std::string& section, const std::string& key) const {
  const auto section_it = data_.find(section);
  if (section_it == data_.end()) {
    return false;
  }
  return section_it->second.find(key) != section_it->second.end();
}

std::string ManimConfig::get(const std::string& section,
                             const std::string& key,
                             const std::string& fallback) const {
  const auto section_it = data_.find(section);
  if (section_it == data_.end()) {
    return fallback;
  }

  const auto key_it = section_it->second.find(key);
  if (key_it == section_it->second.end()) {
    return fallback;
  }

  return key_it->second;
}

void ManimConfig::set(const std::string& section,
                      const std::string& key,
                      const std::string& value) {
  data_[section][key] = value;
}

}  // namespace manim_cpp::config
