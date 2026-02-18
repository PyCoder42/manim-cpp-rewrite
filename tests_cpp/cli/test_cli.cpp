#include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "manim_cpp/cli/cli.hpp"
#include "manim_cpp/scene/registry.hpp"
#include "manim_cpp/scene/scene.hpp"

namespace {

int g_cli_render_scene_runs = 0;

class CliRenderSmokeScene : public manim_cpp::scene::Scene {
 public:
  std::string scene_name() const override { return "CliRenderSmokeScene"; }
  void construct() override { ++g_cli_render_scene_runs; }
};

MANIM_REGISTER_SCENE(CliRenderSmokeScene);

class CliRenderTimedScene : public manim_cpp::scene::Scene {
 public:
  std::string scene_name() const override { return "CliRenderTimedScene"; }
  void construct() override { wait(0.5); }
};

MANIM_REGISTER_SCENE(CliRenderTimedScene);

class ScopedCurrentPath {
 public:
  explicit ScopedCurrentPath(const std::filesystem::path& next_path)
      : previous_(std::filesystem::current_path()) {
    std::filesystem::current_path(next_path);
  }
  ~ScopedCurrentPath() { std::filesystem::current_path(previous_); }

 private:
  std::filesystem::path previous_;
};

class ScopedEnvVar {
 public:
  ScopedEnvVar(std::string name, std::string value) : name_(std::move(name)) {
    const char* current = std::getenv(name_.c_str());
    if (current != nullptr) {
      had_previous_ = true;
      previous_ = current;
    }
    set_value(std::move(value));
  }

  ~ScopedEnvVar() { restore(); }

 private:
  void set_value(std::string value) {
#ifdef _WIN32
    _putenv_s(name_.c_str(), value.c_str());
#else
    setenv(name_.c_str(), value.c_str(), 1);
#endif
  }

  void restore() {
    if (restored_) {
      return;
    }
    restored_ = true;
    if (had_previous_) {
#ifdef _WIN32
      _putenv_s(name_.c_str(), previous_.c_str());
#else
      setenv(name_.c_str(), previous_.c_str(), 1);
#endif
      return;
    }
#ifdef _WIN32
    _putenv_s(name_.c_str(), "");
#else
    unsetenv(name_.c_str());
#endif
  }

  std::string name_;
  std::string previous_;
  bool had_previous_ = false;
  bool restored_ = false;
};

std::string read_file(const std::filesystem::path& path) {
  std::ifstream input(path);
  return std::string((std::istreambuf_iterator<char>(input)),
                     std::istreambuf_iterator<char>());
}

}  // namespace

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

TEST(Cli, RenderHelpIncludesCoreOptions) {
  const std::array<const char*, 3> render_help_args = {
      "manim-cpp", "render", "--help"};

  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code =
      manim_cpp::cli::run_cli(static_cast<int>(render_help_args.size()),
                              render_help_args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find("--renderer <cairo|opengl>"), std::string::npos);
  EXPECT_NE(out_capture.str().find("--format <png|gif|mp4|webm|mov>"),
            std::string::npos);
  EXPECT_NE(out_capture.str().find("--scene <SceneName>"), std::string::npos);
  EXPECT_NE(out_capture.str().find("--watch"), std::string::npos);
  EXPECT_NE(out_capture.str().find("--interactive"), std::string::npos);
  EXPECT_NE(out_capture.str().find("--interaction_script"), std::string::npos);
  EXPECT_NE(out_capture.str().find("--enable_gui"), std::string::npos);
  EXPECT_NE(out_capture.str().find("--window_position"), std::string::npos);
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

TEST(Cli, RenderValidatesRendererOption) {
  const std::array<const char*, 5> valid_args = {
      "manim-cpp", "render", "example_scene.cpp", "--renderer", "opengl"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(valid_args.size()), valid_args.data()),
            0);

  const std::array<const char*, 5> invalid_args = {
      "manim-cpp", "render", "example_scene.cpp", "--renderer", "metal"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(invalid_args.size()), invalid_args.data()),
            2);
}

TEST(Cli, RenderRunsRegisteredSceneWhenRequested) {
  g_cli_render_scene_runs = 0;
  const std::array<const char*, 7> args = {"manim-cpp",
                                            "render",
                                            "example_scene.cpp",
                                            "--scene",
                                            "CliRenderSmokeScene",
                                            "--renderer",
                                            "cairo"};

  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_EQ(g_cli_render_scene_runs, 1);
  EXPECT_NE(out_capture.str().find("Rendered registered scene: CliRenderSmokeScene"),
            std::string::npos);
}

TEST(Cli, RenderFailsForUnknownSceneWhenSceneFlagIsProvided) {
  const std::array<const char*, 5> args = {
      "manim-cpp", "render", "example_scene.cpp", "--scene", "MissingScene"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 2);
}

TEST(Cli, RenderSceneWritesConfiguredMediaArtifacts) {
  g_cli_render_scene_runs = 0;
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_render_scene_outputs";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  std::ofstream cfg(temp_root / "manim.cfg");
  cfg << "[CLI]\n";
  cfg << "media_dir = ./media\n";
  cfg << "video_dir = {media_dir}/videos/{module_name}/{quality}\n";
  cfg << "images_dir = {media_dir}/images/{module_name}\n";
  cfg << "partial_movie_dir = {video_dir}/partial_movie_files/{scene_name}\n";
  cfg << "pixel_width = 1280\n";
  cfg << "pixel_height = 720\n";
  cfg << "frame_rate = 30\n";
  cfg.close();

  std::ofstream input_scene(temp_root / "demo_scene.cpp");
  input_scene << "// placeholder\n";
  input_scene.close();

  {
    ScopedCurrentPath scoped_path(temp_root);
    const std::array<const char*, 9> args = {"manim-cpp",
                                             "render",
                                             "demo_scene.cpp",
                                             "--scene",
                                             "CliRenderSmokeScene",
                                             "--renderer",
                                             "cairo",
                                             "--format",
                                             "webm"};

    std::ostringstream out_capture;
    std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
    const int exit_code =
        manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
    std::cout.rdbuf(old_cout);

    EXPECT_EQ(exit_code, 0);
    EXPECT_EQ(g_cli_render_scene_runs, 1);
    EXPECT_NE(out_capture.str().find("Rendered registered scene: CliRenderSmokeScene"),
              std::string::npos);
    EXPECT_NE(out_capture.str().find("format=webm"), std::string::npos);
    EXPECT_NE(out_capture.str().find("size=1280x720"), std::string::npos);
    EXPECT_NE(out_capture.str().find("fps=30"), std::string::npos);
  }

  const auto media_root = temp_root / "media" / "videos" / "demo_scene" / "720p30";
  const auto output_file = media_root / "CliRenderSmokeScene.webm";
  const auto manifest_file = media_root / "CliRenderSmokeScene.json";
  const auto subcaption_file = media_root / "CliRenderSmokeScene.srt";

  EXPECT_TRUE(std::filesystem::exists(output_file));
  EXPECT_TRUE(std::filesystem::exists(manifest_file));
  EXPECT_TRUE(std::filesystem::exists(subcaption_file));

  const std::string manifest = read_file(manifest_file);
  EXPECT_NE(manifest.find("\"scene\":\"CliRenderSmokeScene\""), std::string::npos);
  EXPECT_NE(manifest.find("\"format\":\"webm\""), std::string::npos);
  EXPECT_NE(manifest.find("\"codec_hint\":\"vp9+opus\""), std::string::npos);
  EXPECT_NE(manifest.find("\"pixel_width\":1280"), std::string::npos);
  EXPECT_NE(manifest.find("\"pixel_height\":720"), std::string::npos);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, RenderSceneReportsCodecHintForRequestedFormat) {
  struct FormatCase {
    std::string format;
    std::string codec_hint;
  };
  const std::vector<FormatCase> cases = {
      {"png", "image/png-sequence"},
      {"gif", "gif"},
      {"mp4", "h264+aac"},
      {"webm", "vp9+opus"},
      {"mov", "prores+pcm"},
  };

  for (const auto& test_case : cases) {
    const auto temp_root = std::filesystem::temp_directory_path() /
                           ("manim_cpp_cli_render_codec_" + test_case.format);
    std::filesystem::remove_all(temp_root);
    std::filesystem::create_directories(temp_root);

    std::ofstream cfg(temp_root / "manim.cfg");
    cfg << "[CLI]\n";
    cfg << "media_dir = ./media\n";
    cfg << "video_dir = {media_dir}/videos/{module_name}/{quality}\n";
    cfg << "images_dir = {media_dir}/images/{module_name}\n";
    cfg << "partial_movie_dir = {video_dir}/partial_movie_files/{scene_name}\n";
    cfg << "pixel_width = 640\n";
    cfg << "pixel_height = 360\n";
    cfg << "frame_rate = 24\n";
    cfg.close();

    std::ofstream input_scene(temp_root / "demo_scene.cpp");
    input_scene << "// placeholder\n";
    input_scene.close();

    {
      ScopedCurrentPath scoped_path(temp_root);
      const std::array<std::string, 9> args_storage = {
          "manim-cpp",
          "render",
          "demo_scene.cpp",
          "--scene",
          "CliRenderSmokeScene",
          "--renderer",
          "cairo",
          "--format",
          test_case.format,
      };
      const std::array<const char*, 9> args = {
          args_storage[0].c_str(),
          args_storage[1].c_str(),
          args_storage[2].c_str(),
          args_storage[3].c_str(),
          args_storage[4].c_str(),
          args_storage[5].c_str(),
          args_storage[6].c_str(),
          args_storage[7].c_str(),
          args_storage[8].c_str(),
      };

      std::ostringstream out_capture;
      std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
      const int exit_code =
          manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
      std::cout.rdbuf(old_cout);

      EXPECT_EQ(exit_code, 0);
      EXPECT_NE(out_capture.str().find("format=" + test_case.format),
                std::string::npos);
      EXPECT_NE(out_capture.str().find("codec_hint=" + test_case.codec_hint),
                std::string::npos);
    }

    const auto media_root =
        temp_root / "media" / "videos" / "demo_scene" / "360p24";
    const auto manifest_file = media_root / "CliRenderSmokeScene.json";
    ASSERT_TRUE(std::filesystem::exists(manifest_file));
    const std::string manifest = read_file(manifest_file);
    EXPECT_NE(manifest.find("\"format\":\"" + test_case.format + "\""),
              std::string::npos);
    EXPECT_NE(manifest.find("\"codec_hint\":\"" + test_case.codec_hint + "\""),
              std::string::npos);

    std::filesystem::remove_all(temp_root);
  }
}

TEST(Cli, RenderSceneWritesDeterministicFrameImagesForCairoRenderer) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_render_scene_frames";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  std::ofstream cfg(temp_root / "manim.cfg");
  cfg << "[CLI]\n";
  cfg << "media_dir = ./media\n";
  cfg << "video_dir = {media_dir}/videos/{module_name}/{quality}\n";
  cfg << "images_dir = {media_dir}/images/{module_name}\n";
  cfg << "partial_movie_dir = {video_dir}/partial_movie_files/{scene_name}\n";
  cfg << "pixel_width = 640\n";
  cfg << "pixel_height = 360\n";
  cfg << "frame_rate = 4\n";
  cfg.close();

  std::ofstream input_scene(temp_root / "demo_scene.cpp");
  input_scene << "// placeholder\n";
  input_scene.close();

  {
    ScopedCurrentPath scoped_path(temp_root);
    const std::array<const char*, 7> args = {"manim-cpp",
                                             "render",
                                             "demo_scene.cpp",
                                             "--scene",
                                             "CliRenderTimedScene",
                                             "--renderer",
                                             "cairo"};

    EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 0);
  }

  const auto images_root = temp_root / "media" / "images" / "demo_scene";
  EXPECT_TRUE(std::filesystem::exists(images_root / "CliRenderTimedScene_000001.png"));
  EXPECT_TRUE(std::filesystem::exists(images_root / "CliRenderTimedScene_000002.png"));
  EXPECT_FALSE(std::filesystem::exists(images_root / "CliRenderTimedScene_000003.png"));

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, RenderSceneWritesDeterministicFrameImagesForOpenGLRenderer) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_render_scene_frames_opengl";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  std::ofstream cfg(temp_root / "manim.cfg");
  cfg << "[CLI]\n";
  cfg << "media_dir = ./media\n";
  cfg << "video_dir = {media_dir}/videos/{module_name}/{quality}\n";
  cfg << "images_dir = {media_dir}/images/{module_name}\n";
  cfg << "partial_movie_dir = {video_dir}/partial_movie_files/{scene_name}\n";
  cfg << "pixel_width = 640\n";
  cfg << "pixel_height = 360\n";
  cfg << "frame_rate = 4\n";
  cfg.close();

  std::ofstream input_scene(temp_root / "demo_scene.cpp");
  input_scene << "// placeholder\n";
  input_scene.close();

  {
    ScopedCurrentPath scoped_path(temp_root);
    const std::array<const char*, 7> args = {"manim-cpp",
                                             "render",
                                             "demo_scene.cpp",
                                             "--scene",
                                             "CliRenderTimedScene",
                                             "--renderer",
                                             "opengl"};

    EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 0);
  }

  const auto images_root = temp_root / "media" / "images" / "demo_scene";
  EXPECT_TRUE(std::filesystem::exists(images_root / "CliRenderTimedScene_000001.png"));
  EXPECT_TRUE(std::filesystem::exists(images_root / "CliRenderTimedScene_000002.png"));
  EXPECT_FALSE(std::filesystem::exists(images_root / "CliRenderTimedScene_000003.png"));

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, RenderSceneManifestIncludesElapsedSectionTimeline) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_render_manifest_timeline";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  std::ofstream cfg(temp_root / "manim.cfg");
  cfg << "[CLI]\n";
  cfg << "media_dir = ./media\n";
  cfg << "video_dir = {media_dir}/videos/{module_name}/{quality}\n";
  cfg << "images_dir = {media_dir}/images/{module_name}\n";
  cfg << "partial_movie_dir = {video_dir}/partial_movie_files/{scene_name}\n";
  cfg << "pixel_width = 640\n";
  cfg << "pixel_height = 360\n";
  cfg << "frame_rate = 4\n";
  cfg.close();

  std::ofstream input_scene(temp_root / "demo_scene.cpp");
  input_scene << "// placeholder\n";
  input_scene.close();

  {
    ScopedCurrentPath scoped_path(temp_root);
    const std::array<const char*, 7> args = {"manim-cpp",
                                             "render",
                                             "demo_scene.cpp",
                                             "--scene",
                                             "CliRenderTimedScene",
                                             "--renderer",
                                             "cairo"};
    EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 0);
  }

  const auto manifest_path = temp_root / "media" / "videos" / "demo_scene" / "360p4" /
                             "CliRenderTimedScene.json";
  ASSERT_TRUE(std::filesystem::exists(manifest_path));
  const std::string manifest = read_file(manifest_path);
  EXPECT_NE(manifest.find("\"start_seconds\":0"), std::string::npos);
  EXPECT_NE(manifest.find("\"end_seconds\":0.5"), std::string::npos);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, RenderAcceptsWatchAndInteractiveFlags) {
  const std::array<const char*, 7> args = {
      "manim-cpp", "render", "example_scene.cpp", "--renderer", "opengl", "--watch",
      "--interactive"};

  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find("watch=true"), std::string::npos);
  EXPECT_NE(out_capture.str().find("interactive=true"), std::string::npos);
  EXPECT_NE(out_capture.str().find("window_open=true"), std::string::npos);
}

TEST(Cli, RenderAppliesInteractionScriptAndReportsCameraState) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_interaction_script";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const auto script_path = temp_root / "camera.script";
  std::ofstream script(script_path);
  script << "# camera replay\n";
  script << "pan_right:2\n";
  script << "pan_up:1.5\n";
  script << "yaw_left:0.5\n";
  script << "zoom_in:0.5\n";
  script.close();

  const auto script_path_string = script_path.string();
  const std::array<const char*, 9> args = {
      "manim-cpp",
      "render",
      "example_scene.cpp",
      "--renderer",
      "opengl",
      "--watch",
      "--interactive",
      "--interaction_script",
      script_path_string.c_str(),
  };

  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find("camera_state=2,1.5,-0.5,0,1.5"), std::string::npos);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, RenderFailsForInvalidInteractionScriptLine) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_interaction_script_bad";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const auto script_path = temp_root / "camera_bad.script";
  std::ofstream script(script_path);
  script << "pan_right:abc\n";
  script.close();

  const auto script_path_string = script_path.string();
  const std::array<const char*, 7> args = {
      "manim-cpp",
      "render",
      "example_scene.cpp",
      "--renderer",
      "opengl",
      "--interaction_script",
      script_path_string.c_str(),
  };

  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 2);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, RenderValidatesFormatOption) {
  const std::array<const char*, 7> valid_args = {"manim-cpp",
                                                  "render",
                                                  "example_scene.cpp",
                                                  "--renderer",
                                                  "cairo",
                                                  "--format",
                                                  "webm"};
  std::ostringstream valid_out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(valid_out_capture.rdbuf());
  const int valid_exit_code =
      manim_cpp::cli::run_cli(static_cast<int>(valid_args.size()), valid_args.data());
  std::cout.rdbuf(old_cout);
  EXPECT_EQ(valid_exit_code, 0);
  EXPECT_NE(valid_out_capture.str().find("format=webm"), std::string::npos);

  const std::array<const char*, 7> invalid_args = {"manim-cpp",
                                                    "render",
                                                    "example_scene.cpp",
                                                    "--renderer",
                                                    "cairo",
                                                    "--format",
                                                    "avi"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(invalid_args.size()),
                                    invalid_args.data()),
            2);
}

TEST(Cli, RenderAcceptsWindowAndGuiOptions) {
  const std::array<const char*, 14> args = {
      "manim-cpp",
      "render",
      "example_scene.cpp",
      "--renderer",
      "opengl",
      "--enable_gui",
      "--fullscreen",
      "--force_window",
      "--window_position",
      "UR",
      "--window_size",
      "1280,720",
      "--window_monitor",
      "2"};

  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find("gui=true"), std::string::npos);
  EXPECT_NE(out_capture.str().find("fullscreen=true"), std::string::npos);
  EXPECT_NE(out_capture.str().find("force_window=true"), std::string::npos);
  EXPECT_NE(out_capture.str().find("window_position=UR"), std::string::npos);
  EXPECT_NE(out_capture.str().find("window_size=1280,720"), std::string::npos);
  EXPECT_NE(out_capture.str().find("window_monitor=2"), std::string::npos);
}

TEST(Cli, RenderRejectsInvalidWindowMonitorValue) {
  const std::array<const char*, 7> args = {"manim-cpp",
                                            "render",
                                            "example_scene.cpp",
                                            "--renderer",
                                            "opengl",
                                            "--window_monitor",
                                            "display-1"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 2);
}

TEST(Cli, RenderRejectsInvalidWindowPositionValue) {
  const std::array<const char*, 7> args = {"manim-cpp",
                                            "render",
                                            "example_scene.cpp",
                                            "--renderer",
                                            "opengl",
                                            "--window_position",
                                            "north-east"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 2);
}

TEST(Cli, RenderRejectsInvalidWindowSizeValue) {
  const std::array<const char*, 7> args = {"manim-cpp",
                                            "render",
                                            "example_scene.cpp",
                                            "--renderer",
                                            "opengl",
                                            "--window_size",
                                            "1280x720"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 2);
}

TEST(Cli, RenderWatchOnlyKeepsWindowClosedByDefault) {
  const std::array<const char*, 6> args = {
      "manim-cpp", "render", "example_scene.cpp", "--renderer", "opengl", "--watch"};

  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find("watch=true"), std::string::npos);
  EXPECT_NE(out_capture.str().find("window_open=false"), std::string::npos);
}

TEST(Cli, AcceptsKnownScaffoldedSubcommands) {
  const std::array<const char*, 3> plugins_invalid_args = {"manim-cpp", "plugins", "bogus"};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(plugins_invalid_args.size()),
                                    plugins_invalid_args.data()),
            2);
}

TEST(Cli, CfgShowAndWriteOperateOnFiles) {
  const auto temp_root = std::filesystem::temp_directory_path() / "manim_cpp_cli_cfg";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const auto source_cfg_path = temp_root / "source.cfg";
  std::ofstream source_cfg(source_cfg_path);
  source_cfg << "[CLI]\nrenderer = cairo\n";
  source_cfg.close();

  const auto source_cfg_string = source_cfg_path.string();
  const std::array<const char*, 4> show_args = {
      "manim-cpp", "cfg", "show", source_cfg_string.c_str()};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(show_args.size()),
                                    show_args.data()),
            0);

  const auto generated_cfg_path = temp_root / "generated.cfg";
  const auto generated_cfg_string = generated_cfg_path.string();
  const std::array<const char*, 4> write_args = {
      "manim-cpp", "cfg", "write", generated_cfg_string.c_str()};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(write_args.size()),
                                    write_args.data()),
            0);
  EXPECT_TRUE(std::filesystem::exists(generated_cfg_path));

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, PluginsListReadsDirectory) {
  const auto temp_root = std::filesystem::temp_directory_path() / "manim_cpp_cli_plugins";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

#ifdef _WIN32
  const auto plugin_path = temp_root / "sample.dll";
#elif __APPLE__
  const auto plugin_path = temp_root / "sample.dylib";
#else
  const auto plugin_path = temp_root / "sample.so";
#endif
  std::ofstream plugin_file(plugin_path);
  plugin_file << "x";
  plugin_file.close();

  const auto path_string = temp_root.string();
  const std::array<const char*, 4> args = {
      "manim-cpp", "plugins", "list", path_string.c_str()};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 0);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, PluginsListDefaultsToResolvedPluginDirWhenDirectoryIsOmitted) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_plugins_default_path";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);
  ScopedEnvVar plugin_env("MANIM_CPP_PLUGIN_DIR", temp_root.string());

#ifdef _WIN32
  const auto plugin_path = temp_root / "sample.dll";
#elif __APPLE__
  const auto plugin_path = temp_root / "sample.dylib";
#else
  const auto plugin_path = temp_root / "sample.so";
#endif
  std::ofstream plugin_file(plugin_path);
  plugin_file << "x";
  plugin_file.close();

  const std::array<const char*, 3> args = {"manim-cpp", "plugins", "list"};

  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find(plugin_path.filename().string()), std::string::npos);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, PluginsLoadAcceptsEmptyDirectory) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_plugins_load_empty";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const auto path_string = temp_root.string();
  const std::array<const char*, 4> args = {
      "manim-cpp", "plugins", "load", path_string.c_str()};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 0);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, PluginsLoadReportsSceneRegistrationsFromPluginInit) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_plugins_load_real";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const std::filesystem::path fixture_path = MANIM_CPP_TEST_PLUGIN_FIXTURE_PATH;
  ASSERT_TRUE(std::filesystem::exists(fixture_path));

  const auto copied_plugin = temp_root / fixture_path.filename();
  ASSERT_TRUE(std::filesystem::copy_file(
      fixture_path, copied_plugin, std::filesystem::copy_options::overwrite_existing));

  const auto path_string = temp_root.string();
  const std::array<const char*, 4> args = {
      "manim-cpp", "plugins", "load", path_string.c_str()};

  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find("Loaded 1 plugin(s)"), std::string::npos);
  EXPECT_NE(out_capture.str().find("FixtureScene"), std::string::npos);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, CheckhealthJsonModeIsAccepted) {
  const std::array<const char*, 3> args = {"manim-cpp", "checkhealth", "--json"};
  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find("\"renderers\":[\"cairo\",\"opengl\"]"),
            std::string::npos);
  EXPECT_NE(out_capture.str().find("\"formats\":[\"png\",\"gif\",\"mp4\",\"webm\",\"mov\"]"),
            std::string::npos);
}

TEST(Cli, CheckhealthJsonReportsPluginDirFromEnvironmentOverride) {
  const auto plugin_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_plugin_override";
  std::filesystem::create_directories(plugin_root);
  ScopedEnvVar plugin_env("MANIM_CPP_PLUGIN_DIR", plugin_root.string());

  const std::array<const char*, 3> args = {"manim-cpp", "checkhealth", "--json"};
  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find(plugin_root.string()), std::string::npos);
}

TEST(Cli, CheckhealthTextModeReportsRenderersAndFormats) {
  const std::array<const char*, 2> args = {"manim-cpp", "checkhealth"};
  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find("renderers: cairo, opengl"), std::string::npos);
  EXPECT_NE(out_capture.str().find("formats: png, gif, mp4, webm, mov"),
            std::string::npos);
}

TEST(Cli, InitSceneGeneratesTemplateFile) {
  const auto temp_root = std::filesystem::temp_directory_path() / "manim_cpp_cli_init";
  std::filesystem::remove_all(temp_root);
  std::filesystem::create_directories(temp_root);

  const auto scene_path = temp_root / "my_scene.cpp";
  const auto scene_path_string = scene_path.string();
  const std::array<const char*, 4> args = {
      "manim-cpp", "init", "scene", scene_path_string.c_str()};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 0);
  EXPECT_TRUE(std::filesystem::exists(scene_path));

  std::ifstream scene_file(scene_path);
  const std::string contents((std::istreambuf_iterator<char>(scene_file)),
                             std::istreambuf_iterator<char>());
  EXPECT_NE(contents.find("MANIM_REGISTER_SCENE"), std::string::npos);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, InitProjectGeneratesProjectScaffold) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_init_project";
  std::filesystem::remove_all(temp_root);

  const auto project_root = temp_root / "demo_project";
  const auto project_root_string = project_root.string();
  const std::array<const char*, 4> args = {
      "manim-cpp", "init", "project", project_root_string.c_str()};
  EXPECT_EQ(manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data()), 0);

  const auto cfg_path = project_root / "manim.cfg";
  const auto scene_path = project_root / "scenes" / "main_scene.cpp";
  EXPECT_TRUE(std::filesystem::exists(cfg_path));
  EXPECT_TRUE(std::filesystem::exists(scene_path));

  std::ifstream scene_file(scene_path);
  const std::string contents((std::istreambuf_iterator<char>(scene_file)),
                             std::istreambuf_iterator<char>());
  EXPECT_NE(contents.find("MANIM_REGISTER_SCENE"), std::string::npos);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, PluginsListSupportsRecursiveDiscoveryFlag) {
  const auto temp_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_plugins_recursive";
  std::filesystem::remove_all(temp_root);
  const auto nested_dir = temp_root / "nested";
  std::filesystem::create_directories(nested_dir);

#ifdef _WIN32
  const auto plugin_path = nested_dir / "sample.dll";
#elif __APPLE__
  const auto plugin_path = nested_dir / "sample.dylib";
#else
  const auto plugin_path = nested_dir / "sample.so";
#endif
  std::ofstream plugin_file(plugin_path);
  plugin_file << "x";
  plugin_file.close();

  const auto path_string = temp_root.string();
  const std::array<const char*, 5> args = {
      "manim-cpp", "plugins", "list", "--recursive", path_string.c_str()};

  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find("sample"), std::string::npos);

  std::filesystem::remove_all(temp_root);
}

TEST(Cli, PluginsPathRespectsEnvironmentOverride) {
  const auto plugin_root =
      std::filesystem::temp_directory_path() / "manim_cpp_cli_plugins_path_override";
  std::filesystem::create_directories(plugin_root);
  ScopedEnvVar plugin_env("MANIM_CPP_PLUGIN_DIR", plugin_root.string());

  const std::array<const char*, 3> args = {"manim-cpp", "plugins", "path"};
  std::ostringstream out_capture;
  std::streambuf* old_cout = std::cout.rdbuf(out_capture.rdbuf());
  const int exit_code = manim_cpp::cli::run_cli(static_cast<int>(args.size()), args.data());
  std::cout.rdbuf(old_cout);

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(out_capture.str().find(plugin_root.string()), std::string::npos);
}
