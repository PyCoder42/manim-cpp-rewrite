#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace manim_cpp::config {

using ConfigSection = std::unordered_map<std::string, std::string>;

class ManimConfig {
 public:
  bool load_from_file(const std::filesystem::path& config_path);
  bool merge_from_file(const std::filesystem::path& config_path);
  bool load_with_precedence(const std::vector<std::filesystem::path>& config_paths);
  void clear();

  bool has(const std::string& section, const std::string& key) const;
  std::string get(const std::string& section,
                  const std::string& key,
                  const std::string& fallback) const;

  void set(const std::string& section,
           const std::string& key,
           const std::string& value);

  const std::unordered_map<std::string, ConfigSection>& data() const {
    return data_;
  }

 private:
  std::unordered_map<std::string, ConfigSection> data_;
};

}  // namespace manim_cpp::config
