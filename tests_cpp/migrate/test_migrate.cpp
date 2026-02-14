#include <string>

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
