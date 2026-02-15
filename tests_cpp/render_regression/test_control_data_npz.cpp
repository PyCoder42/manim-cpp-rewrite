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
