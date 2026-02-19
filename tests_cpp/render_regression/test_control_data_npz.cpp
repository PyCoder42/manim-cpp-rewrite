#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/testing/npz_archive.hpp"

namespace {

std::filesystem::path find_repo_root() {
  auto probe = std::filesystem::current_path();
  for (int depth = 0; depth < 12; ++depth) {
    if (std::filesystem::exists(probe / "tests" / "test_graphical_units" /
                                "control_data")) {
      return probe;
    }
    if (!probe.has_parent_path()) {
      break;
    }
    probe = probe.parent_path();
  }
  return {};
}

}  // namespace

TEST(NpzArchive, ListsEntriesFromKnownControlDataArchive) {
  const auto repo_root = find_repo_root();
  ASSERT_FALSE(repo_root.empty());

  const auto npz_path =
      repo_root / "tests" / "test_graphical_units" / "control_data" / "tables" /
      "IntegerTable.npz";
  ASSERT_TRUE(std::filesystem::exists(npz_path));

  const auto entries = manim_cpp::testing::read_npz_central_directory(npz_path);
  ASSERT_TRUE(entries.has_value());
  ASSERT_FALSE(entries->empty());
  EXPECT_TRUE(manim_cpp::testing::has_npy_entry(*entries));
  EXPECT_EQ(entries->front().name, std::string("frame_data.npy"));
}

TEST(NpzArchive, ParsesAllGraphicalControlDataArchives) {
  const auto repo_root = find_repo_root();
  ASSERT_FALSE(repo_root.empty());

  const auto control_data_root =
      repo_root / "tests" / "test_graphical_units" / "control_data";
  ASSERT_TRUE(std::filesystem::exists(control_data_root));

  std::size_t archive_count = 0;
  for (const auto& entry :
       std::filesystem::recursive_directory_iterator(control_data_root)) {
    if (!entry.is_regular_file() || entry.path().extension() != ".npz") {
      continue;
    }
    ++archive_count;
    const auto entries =
        manim_cpp::testing::read_npz_central_directory(entry.path());
    EXPECT_TRUE(entries.has_value()) << entry.path();
    if (!entries.has_value()) {
      continue;
    }
    EXPECT_FALSE(entries->empty()) << entry.path();
    EXPECT_TRUE(manim_cpp::testing::has_npy_entry(*entries)) << entry.path();
  }

  EXPECT_GT(archive_count, static_cast<std::size_t>(0));
}

TEST(NpzArchive, WritesStoreArchiveRoundTripCentralDirectory) {
  const auto temp_root = std::filesystem::temp_directory_path() / "manim_cpp_npz_writer";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const auto output_path = temp_root / "sample.npz";
  const std::vector<manim_cpp::testing::NpzWriteEntry> write_entries = {
      {
          .name = "alpha.npy",
          .bytes = {0x01, 0x02, 0x03, 0x04},
      },
      {
          .name = "beta.npy",
          .bytes = {0x0A, 0x0B},
      },
  };

  ASSERT_TRUE(manim_cpp::testing::write_npz_store_archive(output_path, write_entries));
  ASSERT_TRUE(std::filesystem::exists(output_path));

  const auto read_entries = manim_cpp::testing::read_npz_central_directory(output_path);
  ASSERT_TRUE(read_entries.has_value());
  ASSERT_EQ(read_entries->size(), static_cast<std::size_t>(2));
  EXPECT_EQ(read_entries->at(0).name, std::string("alpha.npy"));
  EXPECT_EQ(read_entries->at(0).compression_method, static_cast<std::uint16_t>(0));
  EXPECT_EQ(read_entries->at(0).compressed_size, static_cast<std::uint32_t>(4));
  EXPECT_EQ(read_entries->at(0).uncompressed_size, static_cast<std::uint32_t>(4));
  EXPECT_EQ(read_entries->at(1).name, std::string("beta.npy"));
  EXPECT_EQ(read_entries->at(1).compression_method, static_cast<std::uint16_t>(0));
  EXPECT_EQ(read_entries->at(1).compressed_size, static_cast<std::uint32_t>(2));
  EXPECT_EQ(read_entries->at(1).uncompressed_size, static_cast<std::uint32_t>(2));
  EXPECT_TRUE(manim_cpp::testing::has_npy_entry(*read_entries));

  std::filesystem::remove_all(temp_root);
}

TEST(NpzArchive, RejectsDuplicateOrEmptyEntryNamesWhenWriting) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_npz_writer_invalid";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const auto duplicate_path = temp_root / "duplicate.npz";
  const std::vector<manim_cpp::testing::NpzWriteEntry> duplicate_entries = {
      {
          .name = "frame.npy",
          .bytes = {0x01},
      },
      {
          .name = "frame.npy",
          .bytes = {0x02},
      },
  };
  EXPECT_FALSE(
      manim_cpp::testing::write_npz_store_archive(duplicate_path, duplicate_entries));
  EXPECT_FALSE(std::filesystem::exists(duplicate_path));

  const auto empty_name_path = temp_root / "empty_name.npz";
  const std::vector<manim_cpp::testing::NpzWriteEntry> empty_name_entries = {
      {
          .name = "",
          .bytes = {0x01},
      },
  };
  EXPECT_FALSE(
      manim_cpp::testing::write_npz_store_archive(empty_name_path, empty_name_entries));
  EXPECT_FALSE(std::filesystem::exists(empty_name_path));

  std::filesystem::remove_all(temp_root);
}
