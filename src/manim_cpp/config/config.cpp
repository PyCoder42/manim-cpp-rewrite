#include "manim_cpp/config/config.hpp"

#include <fstream>
#include <optional>
#include <string>

namespace manim_cpp::config {
namespace {

constexpr int kMaxTemplateDepth = 16;

std::string trim(const std::string& value) {
  const auto first = value.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) {
    return "";
  }
  const auto last = value.find_last_not_of(" \t\r\n");
  return value.substr(first, last - first + 1);
}

bool parse_config_file(
    const std::filesystem::path& config_path,
    std::unordered_map<std::string, ConfigSection>* output_data) {
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
      (*output_data)[active_section];
      continue;
    }

    const auto equals_pos = cleaned.find('=');
    if (equals_pos == std::string::npos || active_section.empty()) {
      continue;
    }

    const std::string key = trim(cleaned.substr(0, equals_pos));
    const std::string value = trim(cleaned.substr(equals_pos + 1));
    if (!key.empty()) {
      (*output_data)[active_section][key] = value;
    }
  }

  return true;
}

std::optional<std::string> resolve_template_value(
    const ManimConfig& config,
    const std::string& section,
    const std::string& template_value,
    const std::unordered_map<std::string, std::string>& substitutions,
    int depth);

std::optional<std::string> resolve_template_key(
    const ManimConfig& config,
    const std::string& section,
    const std::string& key,
    const std::unordered_map<std::string, std::string>& substitutions,
    const int depth) {
  const auto substitution_it = substitutions.find(key);
  if (substitution_it != substitutions.end()) {
    return substitution_it->second;
  }

  if (config.has(section, key)) {
    return resolve_template_value(config,
                                  section,
                                  config.get(section, key, ""),
                                  substitutions,
                                  depth + 1);
  }
  if (section != "CLI" && config.has("CLI", key)) {
    return resolve_template_value(config,
                                  "CLI",
                                  config.get("CLI", key, ""),
                                  substitutions,
                                  depth + 1);
  }

  return std::nullopt;
}

std::optional<std::string> resolve_template_value(
    const ManimConfig& config,
    const std::string& section,
    const std::string& template_value,
    const std::unordered_map<std::string, std::string>& substitutions,
    const int depth) {
  if (depth > kMaxTemplateDepth) {
    return std::nullopt;
  }

  std::string resolved;
  resolved.reserve(template_value.size() + 32);
  std::size_t cursor = 0;
  while (cursor < template_value.size()) {
    if (template_value[cursor] != '{') {
      resolved.push_back(template_value[cursor]);
      ++cursor;
      continue;
    }

    const auto close = template_value.find('}', cursor + 1);
    if (close == std::string::npos) {
      return std::nullopt;
    }
    const std::string key = trim(template_value.substr(cursor + 1, close - cursor - 1));
    if (key.empty()) {
      return std::nullopt;
    }

    const auto value =
        resolve_template_key(config, section, key, substitutions, depth);
    if (!value.has_value()) {
      return std::nullopt;
    }
    resolved += value.value();
    cursor = close + 1;
  }

  return resolved;
}

}  // namespace

bool ManimConfig::load_from_file(const std::filesystem::path& config_path) {
  clear();
  return merge_from_file(config_path);
}

bool ManimConfig::merge_from_file(const std::filesystem::path& config_path) {
  return parse_config_file(config_path, &data_);
}

bool ManimConfig::load_with_precedence(
    const std::vector<std::filesystem::path>& config_paths) {
  clear();
  for (const auto& path : config_paths) {
    if (!merge_from_file(path)) {
      return false;
    }
  }
  return true;
}

void ManimConfig::clear() {
  data_.clear();
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

std::optional<std::filesystem::path> ManimConfig::resolve_path(
    const std::string& section,
    const std::string& key,
    const std::unordered_map<std::string, std::string>& substitutions) const {
  if (!has(section, key)) {
    return std::nullopt;
  }

  const auto resolved_value =
      resolve_template_value(*this, section, get(section, key, ""), substitutions, 0);
  if (!resolved_value.has_value()) {
    return std::nullopt;
  }
  return std::filesystem::path(resolved_value.value());
}

void ManimConfig::set(const std::string& section,
                      const std::string& key,
                      const std::string& value) {
  data_[section][key] = value;
}

}  // namespace manim_cpp::config
