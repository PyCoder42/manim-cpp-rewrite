#include <array>

#include <gtest/gtest.h>

#include "manim_cpp/cli/cli.hpp"

TEST(Cli, HelpAndSubcommandDispatchSmoke) {
  const std::array<const char*, 2> help_args = {"manim-cpp", "--help"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(help_args.size()),
                                    help_args.data()),
            0);

  const std::array<const char*, 3> render_help_args = {
      "manim-cpp", "render", "--help"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(render_help_args.size()),
                                    render_help_args.data()),
            0);
}

TEST(Cli, ReturnsNonZeroForInvalidSubcommandUsage) {
  const std::array<const char*, 3> cfg_invalid_args = {"manim-cpp", "cfg", "bogus"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(cfg_invalid_args.size()),
                                    cfg_invalid_args.data()),
            2);

  const std::array<const char*, 2> render_missing_args = {"manim-cpp", "render"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(render_missing_args.size()),
                                    render_missing_args.data()),
            2);
}

TEST(Cli, AcceptsKnownScaffoldedSubcommands) {
  const std::array<const char*, 3> plugins_list_args = {"manim-cpp", "plugins", "list"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(plugins_list_args.size()),
                                    plugins_list_args.data()),
            0);
}
