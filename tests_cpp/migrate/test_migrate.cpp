#include <array>
#include <string>
#include <filesystem>
#include <fstream>
#include <iterator>

#include <gtest/gtest.h>

#include "manim_cpp/migrate/migrate.hpp"

TEST(MigrateTool, GeneratesSceneStubsAndTodoMarkers) {
  const std::string source =
      "from manim import *\n"
      "class Demo(Scene):\n"
      "    def construct(self):\n"
      "        x = lambda y: y\n"
      "        self.play(Write(Text('hi')))\n";

  std::string report;
  const std::string converted =
      manim_cpp::migrate::translate_python_scene_to_cpp(source, &report);

  EXPECT_NE(converted.find("class Demo : public Scene"), std::string::npos);
  EXPECT_NE(converted.find("MANIM_REGISTER_SCENE(Demo);"), std::string::npos);
  EXPECT_NE(converted.find("TODO(migrate): dynamic Python patterns detected"),
            std::string::npos);
  EXPECT_NE(converted.find("TODO(migrate): original call -> self.play(Write(Text('hi')))"),
            std::string::npos);
  EXPECT_NE(report.find("scenes_detected=1"), std::string::npos);
}

TEST(MigrateTool, DetectsAdditionalSceneBaseClasses) {
  const std::string source =
      "from manim import *\n"
      "class OrbitDemo(ThreeDScene):\n"
      "    def construct(self):\n"
      "        self.wait(1)\n";

  std::string report;
  const std::string converted =
      manim_cpp::migrate::translate_python_scene_to_cpp(source, &report);

  EXPECT_NE(converted.find("class OrbitDemo : public Scene"), std::string::npos);
  EXPECT_NE(converted.find("MANIM_REGISTER_SCENE(OrbitDemo);"), std::string::npos);
  EXPECT_NE(converted.find("TODO(migrate): original call -> self.wait(1)"),
            std::string::npos);
  EXPECT_NE(report.find("scenes_detected=1"), std::string::npos);
}

TEST(MigrateTool, WritesReportFileWhenRequested) {
  const auto temp_root = std::filesystem::temp_directory_path() / "manim_cpp_migrate_report";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const auto input_path = temp_root / "input.py";
  const auto output_path = temp_root / "output.cpp";
  const auto report_path = temp_root / "report.txt";

  std::ofstream input_file(input_path);
  input_file << "from manim import *\n"
                "class Demo(Scene):\n"
                "    def construct(self):\n"
                "        self.wait(1)\n";
  input_file.close();

  const auto input_path_str = input_path.string();
  const auto output_path_str = output_path.string();
  const auto report_path_str = report_path.string();
  const std::array<const char*, 6> args = {
      "manim-cpp-migrate",
      input_path_str.c_str(),
      "--out",
      output_path_str.c_str(),
      "--report",
      report_path_str.c_str(),
  };

  EXPECT_EQ(manim_cpp::migrate::run_migrate(static_cast<int>(args.size()), args.data()), 0);
  EXPECT_TRUE(std::filesystem::exists(report_path));

  std::ifstream report_file(report_path);
  const std::string report_contents((std::istreambuf_iterator<char>(report_file)),
                                    std::istreambuf_iterator<char>());
  EXPECT_NE(report_contents.find("scenes_detected=1"), std::string::npos);

  std::filesystem::remove_all(temp_root);
}

TEST(MigrateTool, ConvertsDirectoryInputWhenOutDirIsProvided) {
  const auto temp_root = std::filesystem::temp_directory_path() / "manim_cpp_migrate_out_dir";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const auto input_dir = temp_root / "input";
  const auto output_dir = temp_root / "output";
  std::filesystem::create_directories(input_dir);

  {
    std::ofstream first_scene(input_dir / "alpha.py");
    first_scene << "from manim import *\n"
                   "class Alpha(Scene):\n"
                   "    def construct(self):\n"
                   "        self.wait(1)\n";
  }
  {
    std::ofstream second_scene(input_dir / "beta.py");
    second_scene << "from manim import *\n"
                    "class Beta(ThreeDScene):\n"
                    "    def construct(self):\n"
                    "        self.wait(2)\n";
  }

  const auto input_dir_str = input_dir.string();
  const auto output_dir_str = output_dir.string();
  const std::array<const char*, 4> args = {
      "manim-cpp-migrate",
      input_dir_str.c_str(),
      "--out-dir",
      output_dir_str.c_str(),
  };

  EXPECT_EQ(manim_cpp::migrate::run_migrate(static_cast<int>(args.size()), args.data()), 0);
  EXPECT_TRUE(std::filesystem::exists(output_dir / "alpha.cpp"));
  EXPECT_TRUE(std::filesystem::exists(output_dir / "beta.cpp"));

  std::ifstream alpha_file(output_dir / "alpha.cpp");
  const std::string alpha_contents((std::istreambuf_iterator<char>(alpha_file)),
                                   std::istreambuf_iterator<char>());
  EXPECT_NE(alpha_contents.find("MANIM_REGISTER_SCENE(Alpha);"), std::string::npos);

  std::ifstream beta_file(output_dir / "beta.cpp");
  const std::string beta_contents((std::istreambuf_iterator<char>(beta_file)),
                                  std::istreambuf_iterator<char>());
  EXPECT_NE(beta_contents.find("MANIM_REGISTER_SCENE(Beta);"), std::string::npos);

  std::filesystem::remove_all(temp_root);
}

TEST(MigrateTool, SupportsRecursiveDirectoryConversion) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_migrate_recursive";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const auto input_dir = temp_root / "input";
  const auto nested_dir = input_dir / "nested";
  const auto output_dir = temp_root / "output";
  std::filesystem::create_directories(nested_dir);

  {
    std::ofstream nested_scene(nested_dir / "gamma.py");
    nested_scene << "from manim import *\n"
                    "class Gamma(Scene):\n"
                    "    def construct(self):\n"
                    "        self.wait(1)\n";
  }

  const auto input_dir_str = input_dir.string();
  const auto output_dir_str = output_dir.string();
  const std::array<const char*, 5> args = {
      "manim-cpp-migrate",
      input_dir_str.c_str(),
      "--out-dir",
      output_dir_str.c_str(),
      "--recursive",
  };

  EXPECT_EQ(manim_cpp::migrate::run_migrate(static_cast<int>(args.size()), args.data()), 0);
  EXPECT_TRUE(std::filesystem::exists(output_dir / "nested" / "gamma.cpp"));

  std::ifstream gamma_file(output_dir / "nested" / "gamma.cpp");
  const std::string gamma_contents((std::istreambuf_iterator<char>(gamma_file)),
                                   std::istreambuf_iterator<char>());
  EXPECT_NE(gamma_contents.find("MANIM_REGISTER_SCENE(Gamma);"), std::string::npos);

  std::filesystem::remove_all(temp_root);
}
