#include "manim_cpp/testing/npz_archive.hpp"

#include <fstream>
#include <iterator>
#include <utility>

namespace manim_cpp::testing {
namespace {

constexpr std::uint32_t kEndOfCentralDirectorySignature = 0x06054B50;
constexpr std::uint32_t kCentralDirectoryHeaderSignature = 0x02014B50;
constexpr std::size_t kEndOfCentralDirectoryMinSize = 22;
constexpr std::size_t kCentralDirectoryHeaderSize = 46;
constexpr std::size_t kMaxZipCommentSize = 65535;

std::optional<std::vector<std::uint8_t>> read_bytes(
    const std::filesystem::path& path) {
  std::ifstream input(path, std::ios::binary);
  if (!input.is_open()) {
    return std::nullopt;
  }
  return std::vector<std::uint8_t>(std::istreambuf_iterator<char>(input),
                                   std::istreambuf_iterator<char>());
}

std::uint16_t read_u16(const std::vector<std::uint8_t>& bytes,
                       const std::size_t offset) {
  return static_cast<std::uint16_t>(bytes[offset]) |
         (static_cast<std::uint16_t>(bytes[offset + 1]) << 8U);
}

std::uint32_t read_u32(const std::vector<std::uint8_t>& bytes,
                       const std::size_t offset) {
  return static_cast<std::uint32_t>(bytes[offset]) |
         (static_cast<std::uint32_t>(bytes[offset + 1]) << 8U) |
         (static_cast<std::uint32_t>(bytes[offset + 2]) << 16U) |
         (static_cast<std::uint32_t>(bytes[offset + 3]) << 24U);
}

std::optional<std::size_t> find_end_of_central_directory(
    const std::vector<std::uint8_t>& bytes) {
  if (bytes.size() < kEndOfCentralDirectoryMinSize) {
    return std::nullopt;
  }

  const std::size_t search_max = bytes.size() - kEndOfCentralDirectoryMinSize;
  const std::size_t search_min =
      bytes.size() > (kEndOfCentralDirectoryMinSize + kMaxZipCommentSize)
          ? bytes.size() - (kEndOfCentralDirectoryMinSize + kMaxZipCommentSize)
          : 0;

  for (std::size_t cursor = search_max + 1; cursor > search_min;) {
    --cursor;
    if (read_u32(bytes, cursor) == kEndOfCentralDirectorySignature) {
      return cursor;
    }
  }

  return std::nullopt;
}

}  // namespace

std::optional<std::vector<NpzEntry>> read_npz_central_directory(
    const std::filesystem::path& npz_path) {
  const auto bytes_opt = read_bytes(npz_path);
  if (!bytes_opt.has_value()) {
    return std::nullopt;
  }

  const auto& bytes = bytes_opt.value();
  const auto eocd_offset_opt = find_end_of_central_directory(bytes);
  if (!eocd_offset_opt.has_value()) {
    return std::nullopt;
  }

  const std::size_t eocd_offset = eocd_offset_opt.value();
  if (eocd_offset + kEndOfCentralDirectoryMinSize > bytes.size()) {
    return std::nullopt;
  }

  const std::uint16_t total_entries = read_u16(bytes, eocd_offset + 10);
  const std::uint32_t central_dir_size = read_u32(bytes, eocd_offset + 12);
  const std::uint32_t central_dir_offset = read_u32(bytes, eocd_offset + 16);

  if (static_cast<std::uint64_t>(central_dir_offset) +
          static_cast<std::uint64_t>(central_dir_size) >
      bytes.size()) {
    return std::nullopt;
  }

  std::vector<NpzEntry> entries;
  entries.reserve(total_entries);

  std::size_t cursor = central_dir_offset;
  for (std::uint16_t i = 0; i < total_entries; ++i) {
    if (cursor + kCentralDirectoryHeaderSize > bytes.size()) {
      return std::nullopt;
    }
    if (read_u32(bytes, cursor) != kCentralDirectoryHeaderSignature) {
      return std::nullopt;
    }

    const std::uint16_t compression_method = read_u16(bytes, cursor + 10);
    const std::uint32_t compressed_size = read_u32(bytes, cursor + 20);
    const std::uint32_t uncompressed_size = read_u32(bytes, cursor + 24);
    const std::uint16_t file_name_length = read_u16(bytes, cursor + 28);
    const std::uint16_t extra_field_length = read_u16(bytes, cursor + 30);
    const std::uint16_t file_comment_length = read_u16(bytes, cursor + 32);

    const std::size_t name_offset = cursor + kCentralDirectoryHeaderSize;
    const std::size_t next_cursor =
        name_offset + file_name_length + extra_field_length + file_comment_length;
    if (next_cursor > bytes.size()) {
      return std::nullopt;
    }

    NpzEntry entry;
    entry.name = std::string(bytes.begin() + static_cast<std::ptrdiff_t>(name_offset),
                             bytes.begin() +
                                 static_cast<std::ptrdiff_t>(name_offset + file_name_length));
    entry.compression_method = compression_method;
    entry.compressed_size = compressed_size;
    entry.uncompressed_size = uncompressed_size;
    entries.push_back(std::move(entry));

    cursor = next_cursor;
  }

  return entries;
}

bool has_npy_entry(const std::vector<NpzEntry>& entries) {
  for (const auto& entry : entries) {
    if (entry.name.ends_with(".npy")) {
      return true;
    }
  }
  return false;
}

}  // namespace manim_cpp::testing
