#include <array>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace {

#ifdef _WIN32
#define MANIM_CPP_POPEN _popen
#define MANIM_CPP_PCLOSE _pclose
#else
#define MANIM_CPP_POPEN popen
#define MANIM_CPP_PCLOSE pclose
#endif

struct NpyFrameBuffer {
  std::size_t frames = 0;
  std::size_t height = 0;
  std::size_t width = 0;
  std::size_t channels = 0;
  std::size_t data_offset = 0;
};

std::string trim(const std::string& value) {
  const auto first = value.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) {
    return "";
  }
  const auto last = value.find_last_not_of(" \t\r\n");
  return value.substr(first, last - first + 1);
}

std::string shell_escape(const std::string& text) {
  std::string escaped;
  escaped.reserve(text.size() + 4);
  for (const char c : text) {
    if (c == '\\' || c == '"') {
      escaped.push_back('\\');
    }
    escaped.push_back(c);
  }
  return escaped;
}

std::optional<std::vector<std::uint8_t>> unzip_entry(
    const std::filesystem::path& npz_path, const std::string& entry_name) {
  std::ostringstream command;
  command << "unzip -p \"" << shell_escape(npz_path.string()) << "\" \""
          << shell_escape(entry_name) << "\"";
  FILE* pipe = MANIM_CPP_POPEN(command.str().c_str(), "r");
  if (pipe == nullptr) {
    return std::nullopt;
  }

  std::vector<std::uint8_t> bytes;
  std::array<char, 8192> buffer{};
  while (true) {
    const std::size_t read = std::fread(buffer.data(), 1, buffer.size(), pipe);
    if (read == 0) {
      break;
    }
    bytes.insert(bytes.end(), buffer.begin(), buffer.begin() + read);
  }

  const int status = MANIM_CPP_PCLOSE(pipe);
  if (status != 0 || bytes.empty()) {
    return std::nullopt;
  }
  return bytes;
}

std::optional<NpyFrameBuffer> parse_npy_header(
    const std::vector<std::uint8_t>& npy_bytes) {
  if (npy_bytes.size() < 16) {
    return std::nullopt;
  }

  const std::array<std::uint8_t, 6> kMagic = {0x93, 'N', 'U', 'M', 'P', 'Y'};
  if (!std::equal(kMagic.begin(), kMagic.end(), npy_bytes.begin())) {
    return std::nullopt;
  }

  const std::uint8_t major_version = npy_bytes[6];
  std::size_t header_len = 0;
  std::size_t header_start = 0;
  if (major_version == 1) {
    header_len =
        static_cast<std::size_t>(npy_bytes[8]) |
        (static_cast<std::size_t>(npy_bytes[9]) << static_cast<std::size_t>(8));
    header_start = 10;
  } else if (major_version == 2) {
    header_len = static_cast<std::size_t>(npy_bytes[8]) |
                 (static_cast<std::size_t>(npy_bytes[9]) << 8) |
                 (static_cast<std::size_t>(npy_bytes[10]) << 16) |
                 (static_cast<std::size_t>(npy_bytes[11]) << 24);
    header_start = 12;
  } else {
    return std::nullopt;
  }

  if (header_start + header_len > npy_bytes.size()) {
    return std::nullopt;
  }
  const std::size_t data_offset = header_start + header_len;
  const std::string header(
      reinterpret_cast<const char*>(npy_bytes.data() + header_start), header_len);

  std::smatch match;
  if (!std::regex_search(header, match, std::regex("'descr'\\s*:\\s*'([^']+)'")) ||
      match.size() < 2) {
    return std::nullopt;
  }
  if (match[1].str() != "|u1") {
    return std::nullopt;
  }

  if (!std::regex_search(header,
                         match,
                         std::regex("'fortran_order'\\s*:\\s*(True|False)")) ||
      match.size() < 2) {
    return std::nullopt;
  }
  if (match[1].str() != "False") {
    return std::nullopt;
  }

  if (!std::regex_search(header, match, std::regex("'shape'\\s*:\\s*\\(([^\\)]*)\\)")) ||
      match.size() < 2) {
    return std::nullopt;
  }

  std::vector<std::size_t> shape;
  std::stringstream shape_stream(match[1].str());
  std::string token;
  while (std::getline(shape_stream, token, ',')) {
    token = trim(token);
    if (token.empty()) {
      continue;
    }
    try {
      shape.push_back(static_cast<std::size_t>(std::stoull(token)));
    } catch (...) {
      return std::nullopt;
    }
  }

  if (shape.size() != 4) {
    return std::nullopt;
  }
  if (shape[3] != 3 && shape[3] != 4) {
    return std::nullopt;
  }
  if (shape[0] == 0 || shape[1] == 0 || shape[2] == 0) {
    return std::nullopt;
  }

  const std::size_t expected_bytes =
      shape[0] * shape[1] * shape[2] * shape[3];
  if (data_offset + expected_bytes > npy_bytes.size()) {
    return std::nullopt;
  }

  return NpyFrameBuffer{
      .frames = shape[0],
      .height = shape[1],
      .width = shape[2],
      .channels = shape[3],
      .data_offset = data_offset,
  };
}

bool write_frame_ppm(const std::filesystem::path& output_path,
                     const std::uint8_t* frame_data,
                     const std::size_t height,
                     const std::size_t width,
                     const std::size_t channels) {
  std::ofstream output(output_path, std::ios::binary);
  if (!output.is_open()) {
    return false;
  }
  output << "P6\n" << width << " " << height << "\n255\n";

  const std::size_t pixel_count = height * width;
  if (channels == 3) {
    output.write(reinterpret_cast<const char*>(frame_data),
                 static_cast<std::streamsize>(pixel_count * 3));
    return output.good();
  }

  std::array<char, 3> rgb{};
  for (std::size_t i = 0; i < pixel_count; ++i) {
    rgb[0] = static_cast<char>(frame_data[i * channels + 0]);
    rgb[1] = static_cast<char>(frame_data[i * channels + 1]);
    rgb[2] = static_cast<char>(frame_data[i * channels + 2]);
    output.write(rgb.data(), static_cast<std::streamsize>(rgb.size()));
    if (!output.good()) {
      return false;
    }
  }
  return true;
}

void print_usage() {
  std::cout << "Manim-Cpp Graphical Test Frame Extractor\n";
  std::cout << "Usage: manim-cpp-extract-frames <input.npz> <output_directory>\n";
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 3) {
    print_usage();
    return 2;
  }

  const std::filesystem::path input_npz = argv[1];
  const std::filesystem::path output_dir = argv[2];
  std::filesystem::create_directories(output_dir);

  const auto npy_bytes = unzip_entry(input_npz, "frame_data.npy");
  if (!npy_bytes.has_value()) {
    std::cerr << "Unable to extract frame_data.npy from " << input_npz
              << ". Ensure 'unzip' is available and the archive is valid.\n";
    return 1;
  }

  const auto frame_buffer = parse_npy_header(npy_bytes.value());
  if (!frame_buffer.has_value()) {
    std::cerr << "Unsupported or invalid frame_data.npy format in " << input_npz
              << ". Expected C-order uint8 array with shape (N,H,W,3|4).\n";
    return 1;
  }

  const std::size_t frame_size =
      frame_buffer->height * frame_buffer->width * frame_buffer->channels;
  const std::uint8_t* data_start = npy_bytes->data() + frame_buffer->data_offset;

  for (std::size_t frame = 0; frame < frame_buffer->frames; ++frame) {
    const auto output_path = output_dir / ("frame" + std::to_string(frame) + ".ppm");
    const std::uint8_t* frame_ptr = data_start + frame * frame_size;
    if (!write_frame_ppm(output_path,
                         frame_ptr,
                         frame_buffer->height,
                         frame_buffer->width,
                         frame_buffer->channels)) {
      std::cerr << "Failed to write frame file: " << output_path << "\n";
      return 1;
    }
  }

  std::cout << "Saved " << frame_buffer->frames << " frame(s) to " << output_dir
            << " (PPM format).\n";
  return 0;
}
