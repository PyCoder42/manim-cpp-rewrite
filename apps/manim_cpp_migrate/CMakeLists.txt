add_executable(manim-cpp-migrate main.cpp)
target_link_libraries(manim-cpp-migrate PRIVATE manim_cpp_core)
install(TARGETS manim-cpp-migrate)
