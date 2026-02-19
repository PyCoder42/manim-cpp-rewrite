#include "manim_cpp/testing/npz_archive.hpp"

#include <fstream>
#include <iterator>
#include <limits>
#include <unordered_set>
#include <utility>

namespace manim_cpp::testing {
namespace {

constexpr std::uint32_t kEndOfCentralDirectorySignature = 0x06054B50;
constexpr std::uint32_t kCentralDirectoryHeaderSignature = 0x02014B50;
constexpr std::uint32_t kLocalFileHeaderSignature = 0x04034B50;
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

void write_u16(std::ofstream* output, const std::uint16_t value) {
  output->put(static_cast<char>(value & 0xFFU));
  output->put(static_cast<char>((value >> 8U) & 0xFFU));
}

void write_u32(std::ofstream* output, const std::uint32_t value) {
  output->put(static_cast<char>(value & 0xFFU));
  output->put(static_cast<char>((value >> 8U) & 0xFFU));
  output->put(static_cast<char>((value >> 16U) & 0xFFU));
  output->put(static_cast<char>((value >> 24U) & 0xFFU));
}

std::uint32_t crc32(const std::vector<std::uint8_t>& bytes) {
  std::uint32_t crc = 0xFFFFFFFFU;
  for (const auto byte : bytes) {
    crc ^= static_cast<std::uint32_t>(byte);
    for (int bit = 0; bit < 8; ++bit) {
      if ((crc & 1U) != 0U) {
        crc = (crc >> 1U) ^ 0xEDB88320U;
      } else {
        crc >>= 1U;
      }
    }
  }
  return ~crc;
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

bool write_npz_store_archive(const std::filesystem::path& npz_path,
                             const std::vector<NpzWriteEntry>& entries) {
  if (entries.empty()) {
    return false;
  }

  std::unordered_set<std::string> unique_names;
  for (const auto& entry : entries) {
    if (entry.name.empty()) {
      return false;
    }
    if (entry.bytes.size() >
        static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max())) {
      return false;
    }
    if (!unique_names.insert(entry.name).second) {
      return false;
    }
  }

  if (npz_path.has_parent_path()) {
    std::filesystem::create_directories(npz_path.parent_path());
  }
  std::ofstream output(npz_path, std::ios::binary);
  if (!output.is_open()) {
    return false;
  }

  struct CentralDirectoryWriteEntry {
    std::string name;
    std::uint32_t crc32 = 0;
    std::uint32_t size = 0;
    std::uint32_t local_header_offset = 0;
  };
  std::vector<CentralDirectoryWriteEntry> central_entries;
  central_entries.reserve(entries.size());

  for (const auto& entry : entries) {
    const auto local_header_offset = static_cast<std::uint32_t>(output.tellp());
    const auto entry_size = static_cast<std::uint32_t>(entry.bytes.size());
    const auto entry_name_size = static_cast<std::uint16_t>(entry.name.size());
    const auto entry_crc32 = crc32(entry.bytes);

    write_u32(&output, kLocalFileHeaderSignature);
    write_u16(&output, 20);  // version needed to extract
    write_u16(&output, 0);   // general purpose bit flag
    write_u16(&output, 0);   // compression method: store
    write_u16(&output, 0);   // last mod file time
    write_u16(&output, 0);   // last mod file date
    write_u32(&output, entry_crc32);
    write_u32(&output, entry_size);
    write_u32(&output, entry_size);
    write_u16(&output, entry_name_size);
    write_u16(&output, 0);  // extra field length
    output.write(entry.name.data(),
                 static_cast<std::streamsize>(entry.name.size()));
    if (!entry.bytes.empty()) {
      output.write(reinterpret_cast<const char*>(entry.bytes.data()),
                   static_cast<std::streamsize>(entry.bytes.size()));
    }

    if (!output.good()) {
      return false;
    }

    central_entries.push_back(CentralDirectoryWriteEntry{
        .name = entry.name,
        .crc32 = entry_crc32,
        .size = entry_size,
        .local_header_offset = local_header_offset,
    });
  }

  const auto central_directory_offset = static_cast<std::uint32_t>(output.tellp());
  for (const auto& entry : central_entries) {
    write_u32(&output, kCentralDirectoryHeaderSignature);
    write_u16(&output, 20);  // version made by
    write_u16(&output, 20);  // version needed to extract
    write_u16(&output, 0);   // general purpose bit flag
    write_u16(&output, 0);   // compression method: store
    write_u16(&output, 0);   // last mod file time
    write_u16(&output, 0);   // last mod file date
    write_u32(&output, entry.crc32);
    write_u32(&output, entry.size);
    write_u32(&output, entry.size);
    write_u16(&output, static_cast<std::uint16_t>(entry.name.size()));
    write_u16(&output, 0);  // extra field length
    write_u16(&output, 0);  // file comment length
    write_u16(&output, 0);  // disk number start
    write_u16(&output, 0);  // internal file attributes
    write_u32(&output, 0);  // external file attributes
    write_u32(&output, entry.local_header_offset);
    output.write(entry.name.data(),
                 static_cast<std::streamsize>(entry.name.size()));
  }

  if (!output.good()) {
    return false;
  }

  const auto central_directory_size = static_cast<std::uint32_t>(
      static_cast<std::uint32_t>(output.tellp()) - central_directory_offset);
  const auto entry_count = static_cast<std::uint16_t>(central_entries.size());

  write_u32(&output, kEndOfCentralDirectorySignature);
  write_u16(&output, 0);  // number of this disk
  write_u16(&output, 0);  // disk where central directory starts
  write_u16(&output, entry_count);
  write_u16(&output, entry_count);
  write_u32(&output, central_directory_size);
  write_u32(&output, central_directory_offset);
  write_u16(&output, 0);  // .ZIP file comment length

  return output.good();
}

}  // namespace manim_cpp::testing
