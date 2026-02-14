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
