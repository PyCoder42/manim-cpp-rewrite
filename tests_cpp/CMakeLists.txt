cmake_minimum_required(VERSION 3.26)

include(FetchContent)

find_package(GTest QUIET)
if(NOT GTest_FOUND)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()

add_library(
  manim_cpp_test_plugin
  SHARED
  plugin/fixtures/sample_plugin.cpp
)

target_link_libraries(manim_cpp_test_plugin PRIVATE manim_cpp_core)
target_compile_features(manim_cpp_test_plugin PRIVATE cxx_std_23)

add_executable(
  manim_cpp_tests
  unit/test_scene_registry.cpp
  unit/test_config.cpp
  unit/test_camera.cpp
  unit/test_math_core.cpp
  unit/test_eigen_adapter.cpp
  unit/test_triangulation.cpp
  unit/test_path_ops.cpp
  unit/test_isocurve.cpp
  unit/test_graph_layout.cpp
  unit/test_scene_lifecycle.cpp
  unit/test_scene_random.cpp
  unit/test_scene_types.cpp
  unit/test_animation_timeline.cpp
  unit/test_animation_composition.cpp
  unit/test_basic_animations.cpp
  unit/test_scene_file_writer.cpp
  unit/test_media_format.cpp
  unit/test_mobject.cpp
  unit/test_graph_mobject.cpp
  unit/test_geometry_mobjects.cpp
  unit/test_value_tracker.cpp
  unit/test_renderer.cpp
  unit/test_interaction.cpp
  unit/test_shader_paths.cpp
  cli/test_cli.cpp
  plugin/test_plugin_abi.cpp
  plugin/test_plugin_loader.cpp
  plugin/test_plugin_sdk.cpp
  migrate/test_migrate.cpp
  render_regression/test_control_data_npz.cpp
  render_regression/test_bitwise_render_parity.cpp
  integration/test_example_scenes.cpp
)

target_sources(
  manim_cpp_tests
  PRIVATE
    $<TARGET_OBJECTS:manim_cpp_example_scenes>
)

target_link_libraries(
  manim_cpp_tests
  PRIVATE
    manim_cpp_core
    GTest::gtest_main
)

target_compile_features(manim_cpp_tests PRIVATE cxx_std_23)

add_dependencies(manim_cpp_tests manim_cpp_test_plugin)
target_compile_definitions(
  manim_cpp_tests
  PRIVATE
    MANIM_CPP_TEST_PLUGIN_FIXTURE_PATH="$<TARGET_FILE:manim_cpp_test_plugin>"
)

include(GoogleTest)
gtest_discover_tests(manim_cpp_tests)

add_test(
  NAME DocsI18nCatalogCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/docs/check_i18n_catalogs.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME DocsGettextTemplateCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/i18n/extract_gettext.sh
          --check-only
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME DocsToolingCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/docs/build_docs.sh
          --check-only
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME PythonInventoryAllowlistCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_python_inventory.sh
          --allowlist
          ${PROJECT_SOURCE_DIR}/config/python_allowlist.txt
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME LegacyPythonWorkflowModeCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_legacy_python_workflows.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME CxxReleaseWorkflowCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_cxx_release_workflow.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME PluginAbiWorkflowCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_plugin_abi_workflow.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME MilestoneGateReviewCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_milestone_gate_review.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME PackageChannelWorkflowCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_package_channel_workflow.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME PackageManifestProjectUrlCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_package_manifest_project_url.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME DockerTagContractCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_docker_tag_contract.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME LegacyArtifactsPurgeCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_legacy_artifacts_purged.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME FinalCutoverGateCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_final_cutover_gate.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME LegacyDocsPurgeCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_legacy_docs_purged.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME ReadmeCxxConversionCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_readme_cxx.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME BookDocsCoverageCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_book_docs_coverage.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME BookMetadataCxxCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_book_metadata_cxx.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME ChangelogMarkdownCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_changelog_markdown.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME I18nCatalogScopeCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/docs/check_i18n_catalog_scope.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME GettextTemplateScopeCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_gettext_template_scope.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME IssueTemplateCxxCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_issue_templates_cxx.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME PRTemplateCxxCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_pr_templates_cxx.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME CodeqlCxxWorkflowCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_codeql_cpp.sh
          ${PROJECT_SOURCE_DIR}
)

add_test(
  NAME OriginalFeatureCrossTestCoverageCheck
  COMMAND ${PROJECT_SOURCE_DIR}/tools/repo/check_original_feature_cross_tests.sh
          ${PROJECT_SOURCE_DIR}
          ${CMAKE_BINARY_DIR}
)

add_test(
  NAME CliAliasManimVersion
  COMMAND $<TARGET_FILE:manim> --version
)
set_tests_properties(
  CliAliasManimVersion
  PROPERTIES
    PASS_REGULAR_EXPRESSION "manim-cpp v"
)

add_test(
  NAME CliAliasManimceVersion
  COMMAND $<TARGET_FILE:manimce> --version
)
set_tests_properties(
  CliAliasManimceVersion
  PROPERTIES
    PASS_REGULAR_EXPRESSION "manim-cpp v"
)
