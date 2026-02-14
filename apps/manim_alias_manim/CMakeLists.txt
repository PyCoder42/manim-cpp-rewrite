add_executable(manim main.cpp)
target_link_libraries(manim PRIVATE manim_cpp_core)
install(TARGETS manim)
