#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace manim_cpp::testing {

struct NpzEntry {
  std::string name;
  std::uint16_t compression_method = 0;
  std::uint32_t compressed_size = 0;
  std::uint32_t uncompressed_size = 0;
};

struct NpzWriteEntry {
  std::string name;
  std::vector<std::uint8_t> bytes;
};

std::optional<std::vector<NpzEntry>> read_npz_central_directory(
    const std::filesystem::path& npz_path);
bool has_npy_entry(const std::vector<NpzEntry>& entries);
bool write_npz_store_archive(const std::filesystem::path& npz_path,
                             const std::vector<NpzWriteEntry>& entries);

}  // namespace manim_cpp::testing
