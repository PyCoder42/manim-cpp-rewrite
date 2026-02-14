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
